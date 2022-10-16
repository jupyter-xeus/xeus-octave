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

#include "xeus-octave/output.hpp"
#include "xeus-octave/xinterpreter.hpp"

#include <iostream>

namespace xeus_octave::io
{

xoctave_output::xoctave_output(std::string channel) : m_channel(std::move(channel)) {}

xoctave_output::int_type xoctave_output::overflow(xoctave_output::int_type c)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  // Called for each output character.
  if (!traits_type::eq_int_type(c, traits_type::eof()))
  {
    m_output.push_back(traits_type::to_char_type(c));
  }
  return c;
}

std::streamsize xoctave_output::xsputn(char const* s, std::streamsize count)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  // Called for a string of characters.
  m_output.append(s, static_cast<size_t>(count));
  return count;
}

xoctave_output::int_type xoctave_output::sync()
{
  std::lock_guard<std::mutex> lock(m_mutex);
  // Called in case of flush.
  if (!m_output.empty())
  {
    dynamic_cast<xeus_octave::xoctave_interpreter&>(xeus::get_interpreter()).publish_stream(m_channel, m_output);
    m_output.clear();
  }
  return 0;
}

}  // namespace xeus_octave::io
