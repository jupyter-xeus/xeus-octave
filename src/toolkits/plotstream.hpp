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

#ifndef PLOTSTREAM_H
#define PLOTSTREAM_H

#include <octave/graphics.h>

namespace xeus_octave
{

inline int getPlotStream(octave::graphics_object const& o)
{
  return dynamic_cast<figure::properties const&>(o.get_ancestor("figure").get_properties())
    .get___plot_stream__()
    .int_value();
}

inline void setPlotStream(octave::graphics_object& o, int p)
{
  if (o.isa("figure"))
    dynamic_cast<figure::properties&>(o.get_properties()).set___plot_stream__(p);
}

inline void setPlotStream(octave::graphics_object const& o, int p)
{
  // deCONSTify the graphics_object
  auto _go = o;
  setPlotStream(_go, p);
}

}  // namespace xeus_octave

#endif
