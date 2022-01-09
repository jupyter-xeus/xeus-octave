/***************************************************************************
* Copyright (c) 2022, Giulio Girardi
*
* Distributed under the terms of the GNU General Public License v3.
*
* The full license is in the file LICENSE, distributed with this software.
****************************************************************************/

#include "xeus-octave/xinterpreter.hpp"
#include "xeus-octave/output.hpp"

#include <iostream>

namespace xeus_octave::io {

xoctave_output::xoctave_output(std::string channel)
	: m_channel(std::move(channel)) {
}

xoctave_output::int_type xoctave_output::overflow(xoctave_output::int_type c) {
	std::lock_guard<std::mutex> lock(m_mutex);
	// Called for each output character.
	if (!traits_type::eq_int_type(c, traits_type::eof())) {
		m_output.push_back(traits_type::to_char_type(c));
	}
	return c;
}

std::streamsize xoctave_output::xsputn(const char* s, std::streamsize count) {
	std::lock_guard<std::mutex> lock(m_mutex);
	// Called for a string of characters.
	m_output.append(s, count);
	return count;
}

xoctave_output::int_type xoctave_output::sync() {
	std::lock_guard<std::mutex> lock(m_mutex);
	// Called in case of flush.
	if (!m_output.empty()) {
		dynamic_cast<xeus_octave::xoctave_interpreter&>(xeus::get_interpreter())
			.publish_stream(m_channel, m_output);
		m_output.clear();
	}
	return 0;
}

};