/*
 * Copyright (C) 2020 Giulio Girardi.
 *
 * This file is part of xeus-octave.
 *
 * xeus-octave is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * xeus-octave is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xeus-octave.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "xoctave_interpreter.hpp"

#include <octave/defun-dld.h>
#include <octave/error.h>
#include <octave/graphics-handle.h>
#include <octave/graphics-toolkit.h>
#include <octave/graphics.h>
#include <octave/lo-array-errwarn.h>
#include <octave/oct-stream.h>
#include <octave/ov.h>
#include <octave/ovl.h>
#include <octave/parse.h>
#include <octave/pt-stmt.h>
#include <octave/quit.h>
#include <octave/sighandlers.h>
#include <octave/utils.h>
#include <octave/version.h>

#include <cmath>
#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "config.h"
#include "toolkits/notebook.hpp"
#include "toolkits/plotly.hpp"
#include "xeus/xinterpreter.hpp"
#include "xoctave/display.hpp"
#include "input.hpp"

namespace nl = nlohmann;

using namespace octave;

namespace xoctave {

void xoctave_interpreter::do_print_output(bool drawnow) {
	// Print output if necessary
	if (!buf_stderr.str().empty()) {
		publish_stream("stderr", buf_stderr.str());

		// Clear stream
		buf_stderr.str("");
		buf_stderr.clear();
	}

	if (!buf_stdout.str().empty()) {
		publish_stream("stdout", buf_stdout.str());

		// Clear stream
		buf_stdout.str("");
		buf_stdout.clear();
	}

	if (drawnow)
		octave::feval("drawnow");
}

nl::json xoctave_interpreter::execute_request_impl(int /*execution_counter*/,
												   const std::string& code,
												   bool /*silent*/,
												   bool /*store_history*/,
												   nl::json /*user_expressions*/,
												   bool /*allow_stdin*/) {
	int exit_status = 0;
	std::string line;
	std::string error;
	nl::json result;

	parser str_parser(new lexer(code, interpreter));

	// Clear current figure
	root_figure::properties& root_figure = dynamic_cast<root_figure::properties&>(interpreter.get_gh_manager().get_object(0).get_properties());
	root_figure.set_currentfigure(octave_value(NAN));

	do {
		try {
			str_parser.reset();
			exit_status = str_parser.run();

			if (exit_status == 0) {
				auto stmt_list = str_parser.statement_list();

				if (stmt_list) {
					interpreter.get_evaluator().eval(stmt_list, false);
					do_print_output();
				} else if (str_parser.at_end_of_input()) {
					exit_status = EOF;
					break;
				}
			}
		} catch (const interrupt_exception&) {
			interpreter.recover_from_exception();
			do_print_output();
			publish_execution_error("Interrupt exception", "Kernel was interrupted", std::vector<std::string>());
			result["status"] = "error";
		} catch (const index_exception& e) {
			error = e.message();
			error += "\n" + e.stack_trace();
			interpreter.recover_from_exception();
			do_print_output(false);
			publish_execution_error("Index exception", error, std::vector<std::string>());
			result["status"] = "error";
		} catch (const execution_exception& ee) {
			error = ee.message();
			error += "\n" + ee.stack_trace();
			interpreter.get_error_system().save_exception(ee);
			interpreter.recover_from_exception();
			do_print_output(false);
			publish_execution_error("Execution exception", error, std::vector<std::string>());
			result["status"] = "error";
		} catch (const std::bad_alloc&) {
			interpreter.recover_from_exception();
			do_print_output(false);
			publish_execution_error("Out of memory", "Trying to return to prompt", std::vector<std::string>());
			result["status"] = "error";
		}
	} while (exit_status == 0);

	return result;
}

void xoctave_interpreter::configure_impl() {
	interpreter.read_init_files(true);

	interpreter.execute();

	// Fix disp function and clear display function
	interpreter.get_symbol_table().install_built_in_function("display", octave_value());

	// Prepend our override path to have precedence over default m-files
	interpreter.get_load_path().prepend(XOCTAVE_OVERRIDE_PATH);

	interpreter.get_output_system().page_screen_output(true);

	// Register the graphics toolkits
#ifdef NOTEBOOK_TOOLKIT_ENABLED
	interpreter.get_gtk_manager().register_toolkit("notebook");
	interpreter.get_gtk_manager().load_toolkit(octave::graphics_toolkit(new xoctave::notebook_graphics_toolkit(interpreter)));
#endif
	interpreter.get_gtk_manager().register_toolkit("plotly");
	interpreter.get_gtk_manager().load_toolkit(octave::graphics_toolkit(new xoctave::plotly_graphics_toolkit(interpreter)));

	octave::feval("graphics_toolkit", ovl("fltk"));
#ifdef NOTEBOOK_TOOLKIT_ENABLED
	octave::feval("graphics_toolkit", ovl("notebook"));
#else
	octave::feval("graphics_toolkit", ovl("plotly"));
#endif

	// Override the default input system
	input::set_command_editor(input_handler);

	// Create the xoctave package
	auto xoctave_package = interpreter.get_cdef_manager().make_package("xoctave");

	// Register subpackages and classes
	xoctave::display::register_all(xoctave_package, interpreter);

	// Register the xoctave package
	interpreter.get_cdef_manager().register_package(xoctave_package);

	// Redirect output to string
	std::cout.rdbuf(buf_stdout.rdbuf());
	std::cerr.rdbuf(buf_stderr.rdbuf());
}

nl::json xoctave_interpreter::complete_request_impl(const std::string& code,
													int cursor_pos) {
	nl::json result;

	// We are interested only in the code before the cursor
	std::string realcode = code.substr(0, cursor_pos);
	std::string symbol = get_symbol(realcode, cursor_pos);

#ifndef NDEBUG
	std::clog << "Completing: " << symbol << std::endl;
#endif

	octave_value_list completions = octave::feval("completion_matches", octave_value(symbol), 1);

	result["status"] = "ok";

	if (completions.length()) {
		int prefix = 1;

		for (auto completion : completions(0).string_vector_value().std_list()) {
			std::string c = completion.substr(0, strlen(completion.c_str()));

			// Trim leading '\0'
#ifndef NDEBUG
			std::clog << c << std::endl;
#endif
			result["matches"].push_back(c);
		}

#ifndef NDEBUG
		std::clog << result["matches"] << std::endl;
#endif

		result["cursor_start"] = cursor_pos - symbol.length();
		result["cursor_end"] = cursor_pos;
	}

	return result;
}

nl::json xoctave_interpreter::inspect_request_impl(const std::string& code,
												   int cursor_pos,
												   int /*detail_level*/) {
	nl::json result;
	std::string function = get_symbol(code, cursor_pos);

#ifndef NDEBUG
	std::clog << "Inspect: " << function << std::endl;
#endif

	try {
		std::string htext;
		std::string format;

		interpreter.get_help_system().get_help_text(function, htext, format);

		result["found"] = true;

		if (format == "texinfo") {
			octave_value_list help = octave::feval("__makeinfo__", ovl(htext, "html"), 1);

			result["data"]["text/html"] = std::regex_replace(help(0).string_value(), std::regex("<title>.*</title>"), "");
			result["data"]["application/x-texinfo"] = htext;
		} else if (format == "plain text") {
			result["data"]["text/plain"] = htext;
		} else if (format == "html") {
			result["data"]["text/html"] = htext;
		} else {
			result["found"] = false;
		}
	} catch (...) {
		result["found"] = false;
	}

	result["status"] = "ok";

	return result;
}

nl::json xoctave_interpreter::is_complete_request_impl(const std::string& /*code*/) {
	nl::json result;

	result["status"] = "complete";

	return result;
}

nl::json xoctave_interpreter::kernel_info_request_impl() {
	nl::json result;
	result["implementation"] = "xeus-octave";
	result["implementation_version"] = "0.1.0";
	result["language_info"]["name"] = "octave";
	result["language_info"]["version"] = "5.2";
	result["language_info"]["mimetype"] = "text/x-octave";
	result["language_info"]["file_extension"] = ".m";
	result["language_info"]["codemirror_mode"] = "octave";
	result["language_info"]["pygments_lexer"] = "octave";
	result["banner"] = octave_startup_message();
	return result;
}

void xoctave_interpreter::shutdown_request_impl() {
	// Recover the old input system before shutting down the interpreter
	input::reset_command_editor();
	interpreter.shutdown();

#ifndef NDEBUG
	std::clog << "Bye!!" << std::endl;
#endif
}

std::string xoctave_interpreter::get_symbol(const std::string code, int cursor_pos) const {
	if (cursor_pos == (int) code.size())
		cursor_pos = (int) code.size() - 1;

	while (cursor_pos > 0 && (std::isalnum(code.at(cursor_pos)) || code.at(cursor_pos) == '_')) {
#ifndef NDEBUG
		std::clog << "Cursor pos: " << cursor_pos << " - " << code.at(cursor_pos) << std::endl;
#endif
		cursor_pos--;
	}

	int end_pos = cursor_pos ? ++cursor_pos : 0;

	while (end_pos < (int) code.size() && (std::isalnum(code.at(end_pos)) || code.at(end_pos) == '_')) {
#ifndef NDEBUG
		std::clog << "End pos: " << end_pos << " - " << code.at(end_pos) << std::endl;
#endif
		end_pos++;
	}

	return code.substr(cursor_pos, end_pos - cursor_pos);
}

}  // namespace xoctave