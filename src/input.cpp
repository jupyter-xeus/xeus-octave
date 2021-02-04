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

#include "input.hpp"

#include <iostream>

#include "xeus/xinput.hpp"
#include "xoctave_interpreter.hpp"

namespace xoctave {

namespace {

static octave::command_editor* old;

/**
 * Crazy hack to access the private member
 */
template <octave::command_editor** M>
struct AccessInstance {
	friend void _set_command_editor(octave::command_editor& n) {
		// Backup old instance
		old = *M;

		// Override s_instance
		*M = &n;
	}

	friend void _reset_command_editor() {
		*M = old;
	}
};

template struct AccessInstance<&octave::command_editor::s_instance>;

void _set_command_editor(octave::command_editor& n);
void _reset_command_editor();

}  // namespace

void input::set_command_editor(octave::command_editor& n) {
	_set_command_editor(n);
}
void input::reset_command_editor() {
	_reset_command_editor();
}

std::string input::do_readline(const std::string& prompt, bool&) {
	auto& interpreter = dynamic_cast<xoctave_interpreter&>(xeus::get_interpreter());

	if (interpreter.m_allow_stdin) {
		// Print any output if needed
		interpreter.do_print_output();

		// Perform the read request
		auto input = xeus::blocking_input_request(prompt, false);

		// Print newline
		std::cout << std::endl;
		interpreter.do_print_output();

		return input;
	}

	throw std::runtime_error("This frontend does not support input requests");
}

}  // namespace xoctave