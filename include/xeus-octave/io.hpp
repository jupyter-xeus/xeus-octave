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

#ifndef XEUS_OCTAVE_IO_H
#define XEUS_OCTAVE_IO_H

#include <mutex>
#include <ostream>
#include <streambuf>

#include "octave/cmd-edit.h"

namespace xeus_octave
{

class input : public octave::command_editor
{
public:

  input(std::function<std::string(std::string const&)> callback);

  static void override(input& n);
  static void restore();

  std::string do_readline(std::string const& prompt, bool&) override;

  void do_set_input_stream(FILE*) override {}

  FILE* do_get_input_stream(void) override { return nullptr; }

  void do_set_output_stream(FILE*) override {}

  FILE* do_get_output_stream(void) override { return nullptr; }

  string_vector do_generate_filename_completions(std::string const&) override { return {}; }

  std::string do_get_line_buffer(void) const override { return ""; }

  std::string do_get_current_line(void) const override { return ""; }

  char do_get_prev_char(int) const override { return '\0'; }

  void do_replace_line(std::string const&, bool) override {}

  void do_kill_full_line(void) override {}

  void do_insert_text(std::string const&) override {}

  void do_newline(void) override {}

  void do_accept_line(void) override {}

private:

  std::function<std::string(std::string const&)> m_callback;
};

class output : public std::streambuf
{
public:

  output(std::function<void(std::string const&)> callback);

  static void override(std::ostream&, output&);
  static void restore(std::ostream&, output&);

protected:

  int_type overflow(int_type c) override;
  std::streamsize xsputn(char const* s, std::streamsize count) override;
  int_type sync() override;

  std::function<void(std::string const&)> m_callback;
  std::string m_output;
  std::mutex m_mutex;

  std::streambuf* p_oldbuf;
};

}  // namespace xeus_octave

#endif  // XEUS_OCTAVE_IO_H
