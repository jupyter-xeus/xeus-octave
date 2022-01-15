/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XNUMERAL_HPP
#define XEUS_OCTAVE_XNUMERAL_HPP

#include "xeus-octave/xwidgets.hpp"
#include "xeus-octave/xwidgets/xnumber.hpp"
#include "xwidgets/xnumeral.hpp"

namespace xeus_octave::widgets::xnumeral {

using namespace xeus_octave::widgets;
using namespace xeus_octave::widgets::xnumber;

template <typename N>
inline void register_numeral(octave::interpreter& interpreter) {
	using W = xw::numeral<N>;

	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "xnumeral");

	xwidgets_inherit_xnumber<W>(interpreter, cls);

	xwidgets_add_property<W, &W::step>(interpreter, cls, "Step");
	xwidgets_add_property<W, &W::continuous_update>(interpreter, cls, "ContinuousUpdate");
	xwidgets_add_property<W, &W::disabled>(interpreter, cls, "Disabled");
}

inline void register_all(octave::interpreter& interpreter) {
	register_numeral<double>(interpreter);
}

}  // namespace xeus_octave::widgets::xnumeral

#endif