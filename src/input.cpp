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
	// Print any output if needed
	dynamic_cast<xoctave_interpreter&>(xeus::get_interpreter()).do_print_output();

	// Perform the read request
	auto input = xeus::blocking_input_request(prompt, false);

	// Print newline
	std::cout << std::endl;
	dynamic_cast<xoctave_interpreter&>(xeus::get_interpreter()).do_print_output();

	return input;
}

}  // namespace xoctave