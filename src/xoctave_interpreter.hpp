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

#ifndef XOCTAVE_INTERPRETER_H
#define XOCTAVE_INTERPRETER_H

#include <octave/interpreter.h>
#include <octave/oct-stream.h>

#include <nlohmann/json.hpp>
#include <sstream>
#include <xeus/xinterpreter.hpp>

#include "input.hpp"

using nlohmann::json;
using xeus::xinterpreter;

namespace xoctave {

class xoctave_interpreter : public xinterpreter {
private:
	octave::interpreter interpreter;

private:
	void configure_impl() override;

	json execute_request_impl(int execution_counter,
							  const std::string& code,
							  bool silent,
							  bool store_history,
							  json user_expressions,
							  bool allow_stdin) override;

	json complete_request_impl(const std::string& code,
							   int cursor_pos) override;

	json inspect_request_impl(const std::string& code,
							  int cursor_pos,
							  int detail_level) override;

	json is_complete_request_impl(const std::string& code) override;

	json kernel_info_request_impl() override;

	void shutdown_request_impl() override;

public:
	void do_print_output(bool drawnow = true);
	void do_display_data(json data, json metadata = json::object(), json transient = json::object());
	void do_update_display_data(json data, json metadata = json::object(), json transient = json::object());

private:
	std::string get_symbol(const std::string &code, int cursor_pos) const;
	json get_help_for_symbol(const std::string &symbol);

	std::stringstream buf_stdout, buf_stderr;
	input input_handler;

	bool m_silent, m_allow_stdin;
};

}  // namespace xoctave

#endif