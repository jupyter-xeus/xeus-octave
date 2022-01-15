/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XACCORDION_HPP
#define XEUS_OCTAVE_XACCORDION_HPP

#include "xeus-octave/xwidgets.hpp"
#include "xeus-octave/xwidgets/xselection_container.hpp"
#include "xwidgets/xaccordion.hpp"

namespace xeus_octave::widgets::xaccordion {

using namespace xeus_octave::widgets;
using namespace xeus_octave::widgets::xselection_container;

inline void register_all(octave::interpreter& interpreter) {
	using W = xw::accordion;

	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "xaccordion");

	xwidgets_inherit_xselection_container<W>(interpreter, cls);
}

}  // namespace xeus_octave::widgets::xtab

#endif