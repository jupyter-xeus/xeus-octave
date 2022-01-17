/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XPROGRESS_HPP
#define XEUS_OCTAVE_XPROGRESS_HPP

#include "xeus-octave/xwidgets.hpp"
#include "xeus-octave/xwidgets/xnumber.hpp"
#include "xwidgets/xprogress.hpp"

namespace xeus_octave::widgets::xprogress {

using namespace xeus_octave::widgets;
using namespace xeus_octave::widgets::xnumber;

template <typename N>
inline void register_slider(octave::interpreter& interpreter) {
	using W = xw::progress<N>;

	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "xprogress");

	xwidgets_inherit_xnumber<W>(interpreter, cls);

	xwidgets_add_property<W, &W::orientation>(interpreter, cls, "Orientation");
	xwidgets_add_property<W, &W::bar_style>(interpreter, cls, "BarStyle");
	// TODO: style
}

inline void register_all(octave::interpreter& interpreter) {
	register_slider<double>(interpreter);
}

}  // namespace xeus_octave::widgets::xprogress

#endif