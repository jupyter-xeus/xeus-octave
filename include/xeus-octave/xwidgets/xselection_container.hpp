/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XSELECTION_CONTAINER_HPP
#define XEUS_OCTAVE_XSELECTION_CONTAINER_HPP

#include "xeus-octave/xwidgets.hpp"
#include "xeus-octave/xwidgets/xbox.hpp"

namespace xeus_octave::widgets::xselection_container {

using namespace xeus_octave::widgets;
using namespace xeus_octave::widgets::xbox;

template <class W>
inline octave_value_list set_title(const octave_value_list& args, int) {
	get_widget<W>(args(0).classdef_object_value())->set_title(args(1).long_value(), args(2).string_value());
	return ovl();
}

template <class W>
inline void xwidgets_inherit_xselection_container(octave::interpreter& interpreter, octave::cdef_class& cls) {
	xwidgets_inherit_xbox<W>(interpreter, cls);

	xwidgets_add_property<W, &W::_titles>(interpreter, cls, "Titles");
	xwidgets_add_property<W, &W::selected_index>(interpreter, cls, "SelectedIndex");

	xwidgets_add_method(interpreter, cls, "set_title", set_title<W>);
}

}  // namespace xeus_octave::widgets::xselection_container

#endif