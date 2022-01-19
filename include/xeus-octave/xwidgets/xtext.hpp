/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XTEXT_HPP
#define XEUS_OCTAVE_XTEXT_HPP

#include "xeus-octave/xwidgets.hpp"
#include "xeus-octave/xwidgets/xstring.hpp"
#include "xwidgets/xtext.hpp"

namespace xeus_octave::widgets::xtext {

using namespace xeus_octave::widgets;
using namespace xeus_octave::widgets::xstring;

inline void register_all(octave::interpreter& interpreter) {
	using W = xw::text;

	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "xtext");

	xwidgets_inherit_xstring<W>(interpreter, cls);

	xwidgets_add_property<W, &W::disabled>(interpreter, cls, "Disabled");
	xwidgets_add_property<W, &W::continuous_update>(interpreter, cls, "ContinuousUpdate");

	xwidgets_add_callback<W, &W::on_submit>(interpreter, cls, "on_submit");
}

}  // namespace xeus_octave::widgets::xtext

#endif