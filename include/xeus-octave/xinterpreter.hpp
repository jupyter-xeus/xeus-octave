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

#ifndef __EMSCRIPTEN__
#include "xeus-octave/input.hpp"
#endif

#include "xeus-octave/xbuffer.hpp"
#include "xeus-octave/config.hpp"

namespace nl = nlohmann;

namespace xeus_octave
{

  class XEUS_OCTAVE_API xoctave_interpreter : public xeus::xinterpreter
  {
  public:

#ifdef __EMSCRIPTEN__
    xoctave_interpreter();
    virtual ~xoctave_interpreter() = default;

    void publish_stdout(const std::string&);
    void publish_stderr(const std::string&);
#endif

    void publish_stream(std::string const& name, std::string const& text);

    void display_data(
      nl::json data,
      nl::json metadata = nl::json::object(),
      nl::json transient = nl::json::object()
    );

    void update_display_data(
      nl::json data,
      nl::json metadata = nl::json::object(),
      nl::json transient = nl::json::object()
    );

    void publish_execution_result(
      int execution_count,
      nl::json data, nl::json metadata
    );

    void publish_execution_error(
      std::string const& ename,
      std::string const& evalue,
      std::vector<std::string> const& trace_back
    );

  protected:

    void configure_impl() override;

    void execute_request_impl(
      send_reply_callback cb,
      int execution_counter,
      std::string const& code,
      xeus::execute_request_config config,
      nl::json user_expressions
    ) override;

    nl::json complete_request_impl(
      std::string const& code,
      int cursor_pos
    ) override;

    nl::json inspect_request_impl(
      std::string const& code,
      int cursor_pos,
      int detail_level
    ) override;

    nl::json is_complete_request_impl(std::string const& code) override;

    nl::json kernel_info_request_impl() override;

    void shutdown_request_impl() override;

    octave::interpreter m_octave_interpreter;

  protected:
#ifdef __EMSCRIPTEN__
    std::streambuf* p_cout_strbuf;
    std::streambuf* p_cerr_strbuf;
    xoutput_buffer m_cout_buffer;
    xoutput_buffer m_cerr_buffer;
#else
    io::xoctave_output m_stdout{"stdout"};
    io::xoctave_output m_stderr{"stderr"};
    io::xoctave_input m_stdin;
#endif

    bool m_silent{false};
    bool m_allow_stdin{false};
  };

}  // namespace xeus_octave

#endif  // XEUS_OCTAVE_INTERPRETER_H
