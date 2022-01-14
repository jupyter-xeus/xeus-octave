/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XTAB_HPP
#define XEUS_OCTAVE_XTAB_HPP

#include "xeus-octave/xwidgets.hpp"
#include "xeus-octave/xwidgets/xselection_container.hpp"
#include "xwidgets/xtab.hpp"

namespace xeus_octave::widgets::xtab {

using namespace xeus_octave::widgets;
using namespace xeus_octave::widgets::xselection_container;

inline void register_all(octave::interpreter& interpreter) {
	using W = xw::tab;

	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "xtab");

	xwidgets_inherit_xselection_container<W>(interpreter, cls);
}

}  // namespace xeus_octave::widgets::xtab

#endif