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

#ifndef INPUT_H
#define INPUT_H

#include "octave/cmd-edit.h"

namespace xoctave {

class input : public octave::command_editor {
public:
	static void set_command_editor(octave::command_editor &n);
	static void reset_command_editor();

	std::string do_readline(const std::string &prompt, bool &) override;
	void do_set_input_stream(FILE *) override {}
	FILE *do_get_input_stream(void) override { return nullptr; }
	void do_set_output_stream(FILE *) override {}
	FILE *do_get_output_stream(void) override { return nullptr; }
	string_vector do_generate_filename_completions(const std::string &) override { return {}; }
	std::string do_get_line_buffer(void) const override { return ""; }
	std::string do_get_current_line(void) const override { return ""; }
	char do_get_prev_char(int) const override { return '\0'; }
	void do_replace_line(const std::string &, bool) override {}
	void do_kill_full_line(void) override {}
	void do_insert_text(const std::string &) override {}
	void do_newline(void) override {}
	void do_accept_line(void) override {}
};

}  // namespace xoctave

#endif