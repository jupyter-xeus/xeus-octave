/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XOUTPUT_HPP
#define XEUS_OCTAVE_XOUTPUT_HPP

#include "xeus-octave/xwidgets.hpp"
#include "xwidgets/xoutput.hpp"

namespace xeus_octave::widgets::xoutput {

using namespace xeus_octave::widgets;

template <class W>
inline octave_value_list capture(const octave_value_list& args, int) {
	get_widget<W>(args(0).classdef_object_value())->capture();
	return ovl();
}

template <class W>
inline octave_value_list release(const octave_value_list& args, int) {
	get_widget<W>(args(0).classdef_object_value())->release();
	return ovl();
}

inline void register_all(octave::interpreter& interpreter) {
	using W = xw::output;

	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "xoutput");

	xwidgets_add_property<W, &W::msg_id>(interpreter, cls, "MsgId");

	xwidgets_add_method(interpreter, cls, "capture", capture<W>);
	xwidgets_add_method(interpreter, cls, "release", release<W>);
	// TODO add outputs
}

}  // namespace xeus_octave::widgets::xoutput

#endif