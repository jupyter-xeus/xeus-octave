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

#ifndef XEUS_OCTAVE_PLOTSTREAM_H
#define XEUS_OCTAVE_PLOTSTREAM_H

#include <octave/graphics.h>

namespace xeus_octave
{

template <class T> inline T getPlotStream(octave::graphics_object const& o);

/**
 * Retrieve from the graphics object the plot_stream property
 */
template <> inline std::string getPlotStream<std::string>(octave::graphics_object const& o)
{
  octave_value ps =
    dynamic_cast<octave::figure::properties const&>(o.get_ancestor("figure").get_properties()).get___plot_stream__();

  if (ps.is_string())
    return ps.string_value();
  else
    return "";
}

/**
 * Set in the graphics object the plot_stream propert
 */
inline void setPlotStream(octave::graphics_object& o, std::string p)
{
  if (o.isa("figure"))
  {
    auto& fp = dynamic_cast<octave::figure::properties&>(o.get_properties());
    fp.set___plot_stream__(p);
  }
}

/**
 * Set in the graphics object the plot_stream propert (const version)
 */
inline void setPlotStream(octave::graphics_object const& o, std::string p)
{
  // deCONSTify the graphics_object
  auto _go = o;
  setPlotStream(_go, p);
}

}  // namespace xeus_octave

#endif
