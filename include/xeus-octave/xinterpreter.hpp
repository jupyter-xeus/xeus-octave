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

#ifndef XEUS_OCTAVE_INTERPRETER_H
#define XEUS_OCTAVE_INTERPRETER_H

#include <functional>
#include <sstream>

#include <nlohmann/json.hpp>
#include <octave/interpreter.h>
#include <octave/oct-stream.h>
#include <xeus/xinterpreter.hpp>

#include "xeus-octave/io.hpp"

namespace nl = nlohmann;

namespace xeus_octave {

class xoctave_interpreter : public xeus::xinterpreter {
private:
  octave::interpreter interpreter;

private:
  void configure_impl() override;

  nl::json execute_request_impl(
    int execution_counter,
    std::string const& code,
    bool silent,
    bool store_history,
    nl::json user_expressions,
    bool allow_stdin
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
  std::string blocking_input_request(std::string const& prompt, bool password);

private:
  static std::string get_symbol(std::string const& code, std::size_t cursor_pos);
  nl::json get_help_for_symbol(std::string const& symbol);

  output m_stdout{std::bind(&xoctave_interpreter::publish_stream, this, "stdout", std::placeholders::_1)};
  output m_stderr{std::bind(&xoctave_interpreter::publish_stream, this, "stderr", std::placeholders::_1)};
  input m_stdin{std::bind(&xoctave_interpreter::blocking_input_request, this, std::placeholders::_1, false)};

  bool m_silent, m_allow_stdin;
};

}  // namespace xeus_octave

#endif
