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

#ifndef XEUS_OCTAVE_INPUT_HPP
#define XEUS_OCTAVE_INPUT_HPP

#include <octave/cmd-edit.h>

namespace xeus_octave::io
{

class xoctave_input : public octave::command_editor
{
public:

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
};

void register_input(xoctave_input&);

}  // namespace xeus_octave::io

#endif
