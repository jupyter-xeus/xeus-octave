/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XTEXTAREA_HPP
#define XEUS_OCTAVE_XTEXTAREA_HPP

#include "xeus-octave/xwidgets.hpp"
#include "xeus-octave/xwidgets/xstring.hpp"
#include "xwidgets/xtextarea.hpp"

namespace xeus_octave::widgets::xtextarea {

using namespace xeus_octave::widgets;
using namespace xeus_octave::widgets::xstring;

inline void register_all(octave::interpreter& interpreter) {
	using W = xw::textarea;

	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "xtextarea");

	xwidgets_inherit_xstring<W>(interpreter, cls);

	xwidgets_add_property<W, &W::rows>(interpreter, cls, "Rows");
	xwidgets_add_property<W, &W::disabled>(interpreter, cls, "Disabled");
	xwidgets_add_property<W, &W::continuous_update>(interpreter, cls, "ContinuousUpdate");
}

}  // namespace xeus_octave::widgets::xtextarea

#endif