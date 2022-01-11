/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#include "xeus/xinterpreter.hpp"

#include <octave/cdef-package.h>
#include <octave/defun-int.h>
#include <octave/interpreter.h>
#include <octave/oct-map.h>
#include <octave/parse.h>
#include <octave/sighandlers.h>
#include <octave/symtab.h>
#include <octave/version.h>

#include <iostream>
#include <optional>
#include <regex>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "xeus-octave/bindings.hpp"
#include "xeus-octave/display.hpp"
#include "xeus-octave/tk_notebook.hpp"
#include "xeus-octave/xinterpreter.hpp"
#include "xeus-octave/xwidgets.hpp"
#include "xeus/xhelper.hpp"
#include "xeus/xinput.hpp"

namespace nl = nlohmann;

namespace {
/**
 * Extract the help for some symbol and return a json objects containing the
 * text in various mimetypes
 */
std::optional<nl::json> get_help_for_symbol(octave::interpreter& interpreter, std::string name) {
	nl::json result;

	try {
		std::string text;
		std::string format;

		// Get the texinfo help text from the interpreter
		interpreter.get_help_system().get_help_text(name, text, format);

		// Octave gives the help in many formats according to the platform
		if (format == "texinfo") {
			// Generate help using the octave __makeinfo__ function requesting an html output
			octave_value_list help = interpreter.feval(
				"__makeinfo__",
				ovl(text, "html"),
				1  // For getting the return value
			);

			// Extract the HTML documentation
			std::string value = help(0).string_value();
			std::smatch match;
			// Remove the unused portion of the document (everything that's outside the body)
			std::regex_search(value, match, std::regex("<body.*>([^]*)</body>"));
			std::string html = match[1];

			// Jupyter style fixes. This is a little hacky, but jupyter messes up a bit
			// when rendering those tags
			html = std::regex_replace(
				html,
				std::regex("<dd(.*?)>"),
				"<dd $1 style='float:unset;width:unset;font-weight:unset;margin-left:40px'>");
			html = std::regex_replace(
				html,
				std::regex("<dt(.*?)>"),
				"<dt $1 style='float:unset;width:unset;margin-left:0px;'>");

			// Return the help in both formats
			result["text/html"] = html;
			result["application/x-texinfo"] = text;
		} else if (format == "plain text") {
			// Return the help in plain text
			result["text/plain"] = text;
		} else if (format == "html") {
			// Return the help in plain text
			result["text/html"] = text;
		} else {
			return std::nullopt;
		}
	} catch (...) {
		std::clog << "Cannot get help for symbol " << name << std::endl;
		return std::nullopt;
	}

	return result;
}

/**
 * Given a chunk of text find the symbol under the cursor
 */
std::string get_symbol_from_cursor_pos(const std::string& code, int cursor_pos) {
	if (cursor_pos == (int) code.size())
		cursor_pos = (int) code.size() - 1;

	while (cursor_pos > 0 && (std::isalnum(code.at(cursor_pos)) || code.at(cursor_pos) == '_')) {
		cursor_pos--;
	}

	int end_pos = cursor_pos ? ++cursor_pos : 0;

	while (end_pos < (int) code.size() && (std::isalnum(code.at(end_pos)) || code.at(end_pos) == '_')) {
		end_pos++;
	}

	return code.substr(cursor_pos, end_pos - cursor_pos);
}

/**
 * Native binding for getting the xeus-octave override path
 */
octave_value_list override_path(const octave_value_list& args, int /*nargout*/) {
	if (args.length() != 0)
		print_usage();

	return ovl(XOCTAVE_OVERRIDE_PATH "/xeus-octave");
}
}  // namespace

namespace xeus_octave {
namespace interpreter {
void register_all(octave::interpreter& interpreter) {
	XEUS_OCTAVE_LANGUAGE_BINDING(interpreter, "XOCTAVE_OVERRIDE_PATH", override_path);
}
}  // namespace interpreter

xoctave_interpreter::xoctave_interpreter() {
	xeus::register_interpreter(this);
}

nl::json xoctave_interpreter::execute_request_impl(int execution_count,		 // Typically the cell number
												   const std::string& code,	 // Code to execute
												   bool /*silent*/,
												   bool /*store_history*/,
												   nl::json /*user_expressions*/,
												   bool /*allow_stdin*/) {
	// Protocol response (successful)
	nl::json kernel_res = xeus::create_successful_reply();

	// Save output buffers
	std::streambuf* old_stdout = std::cout.rdbuf();
	std::streambuf* old_stderr = std::cerr.rdbuf();

	// Override IO system
	std::cout.rdbuf(&m_stdout);
	std::cerr.rdbuf(&m_stderr);

	// Extract magic ?
	std::string trim = code;
	trim.erase(trim.find_last_not_of(" \n\r\t") + 1);
	if (trim.length() && trim[trim.length() - 1] == '?') {
		// User asked for function help
		// Remove ?
		trim.pop_back();

		// Try getting help
		std::optional<nl::json> data = get_help_for_symbol(interpreter, trim);

		if (data) {
			publish_execution_result(
				execution_count, *data,
				nl::json(nl::json::value_t::object));
		} else {
			// If help is not available show an error message
			publish_execution_error(
				"Execution exception",
				"help: '" + trim + "' not found\n",
				std::vector<std::string>());
		}
	} else {
		// Execute code
		// Instanciate octave parser and lexer
		octave::parser str_parser(new octave::lexer(code, interpreter));

		// Clear current figure
		// This is useful for creating a figure in every cell, otherwise running code
		// in subsequent cells updates a previously displayed figure.
		// The current figure is stored in the properties of the root gh object (id 0)
		root_figure::properties& root_figure = dynamic_cast<root_figure::properties&>(interpreter.get_gh_manager().get_object(0).get_properties());
		root_figure.set_currentfigure(octave_value(NAN));

		// Code evaluation
		int exit_status = 0;
		do {
			try {
				str_parser.reset();
				exit_status = str_parser.run();

				if (exit_status == 0) {
					auto stmt_list = str_parser.statement_list();

					if (stmt_list) {
						interpreter.get_evaluator().eval(stmt_list, false);
					} else if (str_parser.at_end_of_input()) {
						exit_status = EOF;
						break;
					}
				}
			} catch (const octave::interrupt_exception&) {
				// Handle error
				interpreter.recover_from_exception();
				// Protocol reply
				publish_execution_error(
					"Interrupt exception",
					"Kernel was interrupted",
					std::vector<std::string>());
				kernel_res = xeus::create_error_reply();
			} catch (const octave::index_exception& e) {
				// Extract error and stacktrace
				std::string error = e.message();
				error += "\n" + e.stack_trace();
				// Handle error
				interpreter.recover_from_exception();
				// Protocol reply
				publish_execution_error(
					"Index exception",
					error,
					std::vector<std::string>());
				kernel_res = xeus::create_error_reply();
			} catch (const octave::execution_exception& ee) {
				// Extract error and stacktrace
				std::string error = ee.message();
				error += "\n" + ee.stack_trace();
				// Save exception as done by octave
				interpreter.get_error_system().save_exception(ee);
				// Handle error
				interpreter.recover_from_exception();
				// Protocol reply
				publish_execution_error(
					"Execution exception",
					error,
					std::vector<std::string>());
				kernel_res = xeus::create_error_reply();
			} catch (const std::bad_alloc&) {
				// Handle error
				interpreter.recover_from_exception();
				// Protocol reply
				publish_execution_error(
					"Out of memory",
					"Trying to return to prompt",
					std::vector<std::string>());
				kernel_res = xeus::create_error_reply();
			}
		} while (exit_status == 0);
	}

	// Update the figure if present
	interpreter.feval("drawnow");

	// Restore IO system
	std::cout.rdbuf(old_stdout);
	std::cerr.rdbuf(old_stderr);

	return kernel_res;
}

void xoctave_interpreter::configure_impl() {
	// Install signal handlers to listen for CTRL+C
	octave::install_signal_handlers();

	// Set interpreter to read user/global configuration files
	interpreter.read_init_files(true);

	// Initialize interpreter
	interpreter.execute();

	// Fix disp function and clear display function
	// TODO: why is this necessary?
	interpreter.get_symbol_table().install_built_in_function("display", octave_value());

	// Prepend our override path to have precedence over default m-files
	interpreter.get_load_path().prepend(XOCTAVE_OVERRIDE_PATH "/xeus-octave");

	// TODO: explain
	interpreter.get_output_system().page_screen_output(true);

	// Register the graphics toolkits
	xeus_octave::tk::notebook::register_all(interpreter);

	// Setting some toolkit the first time is necessary for some unknown reason
	octave_value_list current_toolkit = interpreter.feval("graphics_toolkit", ovl(), 1);
	interpreter.feval("graphics_toolkit", current_toolkit);
	// Real setting
	interpreter.feval("graphics_toolkit", ovl("notebook"));

	// Register embedded functions
	xeus_octave::display::register_all(interpreter);
	xeus_octave::interpreter::register_all(interpreter);

	// Register the input system
	xeus_octave::io::register_input(m_stdin);

	// Register the widget system
	xeus_octave::widgets::register_all(interpreter);

	// Install version variable
	interpreter.get_symbol_table().install_built_in_function(
		"XOCTAVE",
		new octave_builtin(
			[](const octave_value_list&, int) {
				return ovl(XEUS_OCTAVE_VERSION);
			},
			"XOCTAVE"));
}

nl::json xoctave_interpreter::is_complete_request_impl(const std::string& /*code*/) {
	// Not implemented
	return xeus::create_is_complete_reply("complete");
}

nl::json xoctave_interpreter::complete_request_impl(const std::string& code,
													int cursor_pos) {
	// We are interested only in the code before the cursor
	std::string analysed_code = code.substr(0, cursor_pos);
	std::string symbol = get_symbol_from_cursor_pos(analysed_code, cursor_pos);

	// Retrieve the completions from the interpreter
	octave_value_list completions = interpreter.feval(
		"completion_matches",
		octave_value(symbol),
		1  // For getting the return value
	);

	if (completions.length()) {
		nl::json matches = nl::json::array();

		for (auto completion : completions(0).string_vector_value().std_list()) {
			// Trim leading '\0'
			std::string c = completion.substr(0, strlen(completion.c_str()));
			matches.push_back(c);
		}

		return xeus::create_complete_reply(
			matches,
			cursor_pos - symbol.length(),
			cursor_pos);
	} else {
		// No completion result
		return xeus::create_complete_reply(
			nl::json::array(),
			cursor_pos,
			cursor_pos);
	}
}

nl::json xoctave_interpreter::inspect_request_impl(const std::string& code,
												   int cursor_pos,
												   int /*detail_level*/) {
	// Get the inspected symbol
	std::string symbol = get_symbol_from_cursor_pos(code, cursor_pos);

	// Retrieve help for the symbol
	std::optional<nl::json> data = get_help_for_symbol(interpreter, symbol);

	if (data)
		return xeus::create_inspect_reply(true, *data);
	else
		return xeus::create_inspect_reply(false);
}

void xoctave_interpreter::shutdown_request_impl() {
	// Shutdown the octave interpreter
	interpreter.shutdown();

	std::cout << "Bye!!" << std::endl;
}

nl::json xoctave_interpreter::kernel_info_request_impl() {
	return xeus::create_info_reply(
		"5.3",
		"xoctave",
		XEUS_OCTAVE_VERSION,
		"octave",
		OCTAVE_VERSION,
		"text/x-octave",
		".m",
		"octave",
		"octave",
		"",
		octave_startup_message(),
		false,
		nl::json::array());
}

}  // namespace xeus_octave
