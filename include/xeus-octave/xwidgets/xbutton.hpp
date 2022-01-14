/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XBUTTON_HPP
#define XEUS_OCTAVE_XBUTTON_HPP

#include "xeus-octave/xwidgets.hpp"
#include "xwidgets/xbutton.hpp"

namespace xeus_octave::widgets::xbutton {

using namespace xeus_octave::widgets;
inline void register_all(octave::interpreter& interpreter) {
	using W = xw::button;

	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "xbutton");

	xwidgets_add_property<W, &W::description>(interpreter, cls, "Description");

	xwidgets_add_property<W, &W::tooltip>(interpreter, cls, "Tooltip");
	xwidgets_add_property<W, &W::disabled>(interpreter, cls, "Disabled");
	xwidgets_add_property<W, &W::icon>(interpreter, cls, "Icon");
	xwidgets_add_property<W, &W::button_style>(interpreter, cls, "ButtonStyle");
	// TODO: style

	xwidgets_add_callback<W, &W::on_click>(interpreter, cls, "on_click");
}

}  // namespace xeus_octave::widgets::xbutton

#endif