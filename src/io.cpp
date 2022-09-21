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

#include <ostream>
#include <ostream>
#include <mutex>

#include <xeus/xinput.hpp>

#include "xeus-octave/xinterpreter.hpp"
#include "xeus-octave/io.hpp"


namespace xeus_octave {

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

void input::override(input& n) {
	_set_command_editor(n);
}

void input::restore() {
	_reset_command_editor();
}

input::input(std::function<std::string(const std::string&)> callback) : m_callback(std::move(callback)) {}

std::string input::do_readline(const std::string& prompt, bool&) {
	return m_callback(prompt);
}

void output::override(std::ostream& stream, output& buf) {
	// Backup previous buffer
	buf.p_oldbuf = stream.rdbuf();

	stream.rdbuf(&buf);
}

void output::restore(std::ostream& stream, output& buf) {
	stream.rdbuf(buf.p_oldbuf);
}

output::output(std::function<void(const std::string&)> callback)
	: m_callback(std::move(callback)) {
}

output::int_type output::overflow(output::int_type c) {
	std::lock_guard<std::mutex> lock(m_mutex);
	// Called for each output character.
	if (!traits_type::eq_int_type(c, traits_type::eof())) {
		m_output.push_back(traits_type::to_char_type(c));
	}
	return c;
}

std::streamsize output::xsputn(const char* s, std::streamsize count) {
	std::lock_guard<std::mutex> lock(m_mutex);
	// Called for a string of characters.
	m_output.append(s, count);
	return count;
}

output::int_type output::sync() {
	std::lock_guard<std::mutex> lock(m_mutex);
	// Called in case of flush.
	if (!m_output.empty()) {
		m_callback(m_output);
		m_output.clear();
	}
	return 0;
}

}  // namespace xeus_octave
