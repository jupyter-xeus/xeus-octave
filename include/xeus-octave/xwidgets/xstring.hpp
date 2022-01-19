/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XSTRING_HPP
#define XEUS_OCTAVE_XSTRING_HPP

#include "xeus-octave/xwidgets.hpp"
#include "xwidgets/xstring.hpp"

namespace xeus_octave::widgets::xstring {

using namespace xeus_octave::widgets;

template <class W>
inline void xwidgets_inherit_xstring(octave::interpreter& interpreter, octave::cdef_class& cls) {
	xwidgets_add_property<W, &W::description>(interpreter, cls, "Description");
	xwidgets_add_property<W, &W::value>(interpreter, cls, "Value");
	xwidgets_add_property<W, &W::placeholder>(interpreter, cls, "Placeholder");
}

}  // namespace xeus_octave::widgets::xstring

#endif