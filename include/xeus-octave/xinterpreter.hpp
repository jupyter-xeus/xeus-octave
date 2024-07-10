/*
 * Copyright (C) 2022 Giulio Girardi.
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

#ifndef XEUS_OCTAVE_INTERPRETER_H
#define XEUS_OCTAVE_INTERPRETER_H

#include <nlohmann/json.hpp>
#include <octave/interpreter.h>
#include <octave/oct-stream.h>
#include <xeus/xinterpreter.hpp>

#include "xeus-octave/input.hpp"
#include "xeus-octave/output.hpp"

namespace nl = nlohmann;

namespace xeus_octave
{

class xoctave_interpreter : public xeus::xinterpreter
{
private:

  octave::interpreter interpreter;

private:

  void configure_impl() override;

  void execute_request_impl(
    send_reply_callback cb,
    int execution_counter,
    std::string const& code,
    xeus::execute_request_config config,
    nl::json user_expressions
  ) override;

  nl::json complete_request_impl(std::string const& code, int cursor_pos) override;

  nl::json inspect_request_impl(std::string const& code, int cursor_pos, int detail_level) override;

  nl::json is_complete_request_impl(std::string const& code) override;

  nl::json kernel_info_request_impl() override;

  void shutdown_request_impl() override;

public:

  void publish_stream(std::string const& name, std::string const& text);
  void display_data(nl::json data, nl::json metadata = nl::json::object(), nl::json transient = nl::json::object());
  void
  update_display_data(nl::json data, nl::json metadata = nl::json::object(), nl::json transient = nl::json::object());
  void publish_execution_result(int execution_count, nl::json data, nl::json metadata);
  void publish_execution_error(
    std::string const& ename, std::string const& evalue, std::vector<std::string> const& trace_back
  );

private:

  io::xoctave_output m_stdout{"stdout"};
  io::xoctave_output m_stderr{"stderr"};
  io::xoctave_input m_stdin;

  bool m_silent, m_allow_stdin;
};

}  // namespace xeus_octave

#endif  // XEUS_OCTAVE_INTERPRETER_H
