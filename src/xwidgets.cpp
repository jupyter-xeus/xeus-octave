/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#include "xeus-octave/xwidgets.hpp"

#include <octave/interpreter.h>
#include <octave/ov-classdef.h>

#include <iomanip>
#include <iostream>
#include <typeinfo>

#include "xeus-octave/plotstream.hpp"
#include "xeus-octave/utils.hpp"
#include "xeus-octave/xwidgets/xbox.hpp"
#include "xeus-octave/xwidgets/xmedia.hpp"
#include "xeus-octave/xwidgets/xnumber.hpp"
#include "xwidgets/xbox.hpp"
#include "xwidgets/xbutton.hpp"
#include "xwidgets/xslider.hpp"

namespace {

using namespace xeus_octave::widgets::utils;
using namespace xeus_octave::widgets::xnumber::utils;
using namespace xeus_octave::widgets::xmedia::utils;
using namespace xeus_octave::widgets::xbox::utils;

template <typename N>
void register_slider(octave::interpreter& interpreter) {
	using W = xw::slider<N>;

	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "xslider");

	xwidgets_inherit_xnumber<W>(interpreter, cls);

	xwidgets_add_property<W, &W::step>(interpreter, cls, "Step");
	xwidgets_add_property<W, &W::orientation>(interpreter, cls, "Orientation");
	xwidgets_add_property<W, &W::readout>(interpreter, cls, "Readout");
	xwidgets_add_property<W, &W::readout_format>(interpreter, cls, "ReadoutFormat");
	xwidgets_add_property<W, &W::continuous_update>(interpreter, cls, "ContinuousUpdate");
	xwidgets_add_property<W, &W::disabled>(interpreter, cls, "Disabled");
	// TODO: style
}

void register_button(octave::interpreter& interpreter) {
	using W = xw::button;

	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "xbutton");

	xwidgets_add_property<W, &W::description>(interpreter, cls, "Description");

	xwidgets_add_property<W, &W::tooltip>(interpreter, cls, "Tooltip");
	xwidgets_add_property<W, &W::disabled>(interpreter, cls, "Disabled");
	xwidgets_add_property<W, &W::icon>(interpreter, cls, "Icon");
	xwidgets_add_property<W, &W::button_style>(interpreter, cls, "ButtonStyle");
	// TODO: style

	xwidgets_add_callback<W, &W::on_click>(interpreter, cls, "on_click");
}

void register_image(octave::interpreter& interpreter) {
	using W = xw::image;

	octave::cdef_manager& cm = interpreter.get_cdef_manager();
	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "ximage");

	xwidgets_inherit_xmedia<W>(interpreter, cls);

	xwidgets_add_property<W, &W::format>(interpreter, cls, "Format");
	xwidgets_add_property<W, &W::width>(interpreter, cls, "Width");
	xwidgets_add_property<W, &W::height>(interpreter, cls, "Height");

	// Register __pointer__ property for xfigure access
	cls.install_property(cm.make_property(
		cls, XWIDGETS_POINTER_PROPERTY,
		Matrix(), "protected",
		Matrix(), "private"));
}

void register_hbox(octave::interpreter& interpreter) {
	using W = xw::hbox;

	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "xhbox");

	xwidgets_inherit_xbox<W>(interpreter, cls);
}

void register_vbox(octave::interpreter& interpreter) {
	using W = xw::vbox;

	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "xvbox");

	xwidgets_inherit_xbox<W>(interpreter, cls);
}

}  // namespace

namespace xeus_octave::widgets {

void register_all(octave::interpreter& interpreter) {
	register_slider<double>(interpreter);
	register_button(interpreter);
	register_image(interpreter);
	register_hbox(interpreter);
	register_vbox(interpreter);
}

}  // namespace xeus_octave::widgets