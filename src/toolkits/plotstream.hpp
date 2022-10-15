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

#ifndef XEUS_OCTAVE_PLOTSTREAM_H
#define XEUS_OCTAVE_PLOTSTREAM_H

#include <octave/graphics.h>

namespace xeus_octave
{

inline std::string getPlotStream(octave::graphics_object const& o)
{
  return dynamic_cast<octave::figure::properties const&>(o.get_ancestor("figure").get_properties())
    .get___plot_stream__()
    .string_value();
}

inline void setPlotStream(octave::graphics_object& o, std::string p)
{
  if (o.isa("figure"))
    dynamic_cast<octave::figure::properties&>(o.get_properties()).set___plot_stream__(p);
}

inline void setPlotStream(octave::graphics_object const& o, std::string p)
{
  // deCONSTify the graphics_object
  auto _go = o;
  setPlotStream(_go, p);
}

}  // namespace xeus_octave

#endif  // XEUS_OCTAVE_PLOTSTREAM_H
