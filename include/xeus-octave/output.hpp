/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_OUTPUT_HPP
#define XEUS_OCTAVE_OUTPUT_HPP

#include <functional>
#include <mutex>
#include <streambuf>

namespace xeus_octave::io {

class xoctave_output : public std::streambuf {
public:
	xoctave_output(std::string channel);

protected:
	int_type overflow(int_type c) override;
	std::streamsize xsputn(const char *s, std::streamsize count) override;
	int_type sync() override;

	std::string m_channel;
	std::string m_output;
	std::mutex m_mutex;
};

}  // namespace xeus_octave::io

#endif