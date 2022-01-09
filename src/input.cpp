/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#include "xeus-octave/input.hpp"

#include <octave/cmd-edit.h>

#include "xeus-octave/xinterpreter.hpp"

namespace {

static octave::command_editor *old;

/**
 * Crazy hack to access the private member
 */
template <octave::command_editor **M>
struct AccessInstance {
	friend void _set_command_editor(octave::command_editor &n) {
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

void _set_command_editor(octave::command_editor &n);
void _reset_command_editor();

}  // namespace

namespace xeus_octave::io {

std::string xoctave_input::do_readline(const std::string &prompt, bool &) {
	// Interpreter reference
	xeus_octave::xoctave_interpreter &interpreter = dynamic_cast<xeus_octave::xoctave_interpreter &>(xeus::get_interpreter());
	// Read value
	std::string value;

	// Register a input handler (called when input is ready)
	interpreter.register_input_handler([&value](const std::string &v) { value = v; });

	// Send the input request
	interpreter.input_request(prompt, false);

	// Remove input handler
	interpreter.register_input_handler(nullptr);

	return value;
}

void register_input(xoctave_input &input) {
	_set_command_editor(input);
}

}  // namespace xeus_octave::io