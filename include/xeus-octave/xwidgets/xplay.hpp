/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XPLAY_HPP
#define XEUS_OCTAVE_XPLAY_HPP

#include "xeus-octave/xwidgets.hpp"
#include "xeus-octave/xwidgets/xnumber.hpp"
#include "xwidgets/xplay.hpp"

namespace xeus_octave::widgets::xplay {

using namespace xeus_octave::widgets;
using namespace xeus_octave::widgets::xnumber;

inline void register_all(octave::interpreter& interpreter) {
	using W = xw::play;

	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "xplay");

	xwidgets_inherit_xnumber<W>(interpreter, cls);

	xwidgets_add_property<W, &W::interval>(interpreter, cls, "Interval");
	xwidgets_add_property<W, &W::step>(interpreter, cls, "Step");
	xwidgets_add_property<W, &W::disabled>(interpreter, cls, "Disabled");
	xwidgets_add_property<W, &W::_playing>(interpreter, cls, "Playing");
	xwidgets_add_property<W, &W::_repeat>(interpreter, cls, "Repeat");
	xwidgets_add_property<W, &W::show_repeat>(interpreter, cls, "ShowRepeat");
}

}  // namespace xeus_octave::widgets::xplay

#endif