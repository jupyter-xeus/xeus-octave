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

#include "xwidgets/ximage.hpp"

namespace xeus_octave {

template <class T>
inline T getPlotStream(const graphics_object& o);

/**
 * Retrieve from the graphics object the plot_stream property
 */
template <>
inline xw::image* getPlotStream<xw::image*>(const graphics_object& o) {
	octave_value ps = dynamic_cast<const figure::properties&>(o.get_ancestor("figure").get_properties()).get___plot_stream__();

	if (ps.isnumeric() && ps.is_scalar_type())
		return reinterpret_cast<xw::image*>(ps.long_value());
	else
		return nullptr;
}

/**
 * Retrieve from the graphics object the plot_stream property
 */
template <>
inline std::string getPlotStream<std::string>(const graphics_object& o) {
	octave_value ps = dynamic_cast<const figure::properties&>(o.get_ancestor("figure").get_properties()).get___plot_stream__();

	if (ps.is_string())
		return ps.string_value();
	else
		return "";
}

/**
 * Set in the graphics object the plot_stream propert
 */
inline void setPlotStream(graphics_object& o, xw::image* p) {
	if (o.isa("figure")) {
		figure::properties& fp = dynamic_cast<figure::properties&>(o.get_properties());
		fp.set___plot_stream__(reinterpret_cast<intptr_t>(p));
	}
}

/**
 * Set in the graphics object the plot_stream propert
 */
inline void setPlotStream(graphics_object& o, std::string p) {
	if (o.isa("figure")) {
		figure::properties& fp = dynamic_cast<figure::properties&>(o.get_properties());
		fp.set___plot_stream__(p);
	}
}

/**
 * Set in the graphics object the plot_stream propert (const version)
 */
inline void setPlotStream(const graphics_object& o, xw::image* p) {
	// deCONSTify the graphics_object
	auto _go = o;
	setPlotStream(_go, p);
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