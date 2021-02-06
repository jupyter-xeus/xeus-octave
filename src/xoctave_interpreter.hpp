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

#include <functional>
#include <nlohmann/json.hpp>
#include <sstream>
#include <xeus/xinterpreter.hpp>

#include "io.hpp"

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
	void publish_stream(const std::string& name, const std::string& text);
	void display_data(json data, json metadata = json::object(), json transient = json::object());
	void update_display_data(json data, json metadata = json::object(), json transient = json::object());
	void publish_execution_result(int execution_count, nl::json data, nl::json metadata);
	void publish_execution_error(const std::string& ename,
								 const std::string& evalue,
								 const std::vector<std::string>& trace_back);
	std::string blocking_input_request(const std::string& prompt, bool password);

private:
	std::string get_symbol(const std::string& code, int cursor_pos) const;
	json get_help_for_symbol(const std::string& symbol);

	output m_stdout{std::bind(&xoctave_interpreter::publish_stream, this, "stdout", std::placeholders::_1)};
	output m_stderr{std::bind(&xoctave_interpreter::publish_stream, this, "stderr", std::placeholders::_1)};
	input m_stdin{std::bind(&xoctave_interpreter::blocking_input_request, this, std::placeholders::_1, false)};

	bool m_silent, m_allow_stdin;
};

}  // namespace xoctave

#endif
