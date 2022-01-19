/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XLABEL_HPP
#define XEUS_OCTAVE_XLABEL_HPP

#include "xeus-octave/xwidgets.hpp"
#include "xeus-octave/xwidgets/xstring.hpp"
#include "xwidgets/xlabel.hpp"

namespace xeus_octave::widgets::xlabel {

using namespace xeus_octave::widgets;
using namespace xeus_octave::widgets::xstring;

inline void register_all(octave::interpreter& interpreter) {
	using W = xw::label;

	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "xxlabel");

	xwidgets_inherit_xstring<W>(interpreter, cls);
}

}  // namespace xeus_octave::widgets::xlabel

#endif