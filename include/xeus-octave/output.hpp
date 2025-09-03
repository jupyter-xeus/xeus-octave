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

#ifndef XEUS_OCTAVE_OUTPUT_HPP
#define XEUS_OCTAVE_OUTPUT_HPP

#include <mutex>
#include <streambuf>
#include <string>

namespace xeus_octave::io
{

class xoctave_output : public std::streambuf
{
public:

  xoctave_output(std::string channel);

protected:

  int_type overflow(int_type c) override;
  std::streamsize xsputn(char const* s, std::streamsize count) override;
  int_type sync() override;

  std::string m_channel;
  std::string m_output;
  std::mutex m_mutex;
};

}  // namespace xeus_octave::io

#endif
