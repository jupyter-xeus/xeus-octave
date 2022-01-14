/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XNUMBER_HPP
#define XEUS_OCTAVE_XNUMBER_HPP

#include <xeus-octave/xwidgets.hpp>

namespace xeus_octave::widgets::xnumber {

using namespace xeus_octave::widgets;

template <class W>
inline void xwidgets_inherit_xnumber(octave::interpreter& interpreter, octave::cdef_class& cls) {
	xwidgets_add_property<W, &W::description>(interpreter, cls, "Description");

	xwidgets_add_property<W, &W::value>(interpreter, cls, "Value");
	xwidgets_add_property<W, &W::min>(interpreter, cls, "Min");
	xwidgets_add_property<W, &W::max>(interpreter, cls, "Max");
}

}  // namespace xeus_octave::widgets::xnumber

#endif