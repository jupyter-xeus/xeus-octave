/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XMEDIA_HPP
#define XEUS_OCTAVE_XMEDIA_HPP

#include <xeus-octave/xwidgets.hpp>

namespace xeus_octave::widgets::xmedia::utils {

using namespace xeus_octave::widgets::utils;

template <class W>
inline void xwidgets_inherit_xmedia(octave::interpreter& interpreter, octave::cdef_class& cls) {
	xwidgets_add_property<W, &W::value>(interpreter, cls, "Value");
}

}  // namespace xeus_octave::widgets::xmedia::utils

#endif