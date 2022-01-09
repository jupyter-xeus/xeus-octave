/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_PLOTSTREAM_HPP
#define XEUS_OCTAVE_PLOTSTREAM_HPP

#include <octave/graphics.h>

namespace xeus_octave {

/**
 * Retrieve from the graphics object the plot_stream property
 */
inline std::string getPlotStream(const graphics_object& o) {
	return dynamic_cast<const figure::properties&>(o.get_ancestor("figure").get_properties())
		.get___plot_stream__()
		.string_value();
}

/**
 * Set in the graphics object the plot_stream propert
 */
inline void setPlotStream(graphics_object& o, std::string p) {
	if (o.isa("figure"))
		dynamic_cast<figure::properties&>(o.get_properties()).set___plot_stream__(p);
}

/**
 * Set in the graphics object the plot_stream propert (const version)
 */
inline void setPlotStream(const graphics_object& o, std::string p) {
	// deCONSTify the graphics_object
	auto _go = o;
	setPlotStream(_go, p);
}

}  // namespace xeus_octave

#endif