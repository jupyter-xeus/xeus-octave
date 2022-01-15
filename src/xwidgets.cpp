/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#include "xeus-octave/xwidgets.hpp"

#include <octave/interpreter.h>

#include <iomanip>
#include <iostream>
#include <typeinfo>

#include "xeus-octave/xwidgets/xaccordion.hpp"
#include "xeus-octave/xwidgets/xaudio.hpp"
#include "xeus-octave/xwidgets/xbox.hpp"
#include "xeus-octave/xwidgets/xbutton.hpp"
#include "xeus-octave/xwidgets/ximage.hpp"
#include "xeus-octave/xwidgets/xnumeral.hpp"
#include "xeus-octave/xwidgets/xoutput.hpp"
#include "xeus-octave/xwidgets/xplay.hpp"
#include "xeus-octave/xwidgets/xprogress.hpp"
#include "xeus-octave/xwidgets/xslider.hpp"
#include "xeus-octave/xwidgets/xtab.hpp"
#include "xeus-octave/xwidgets/xvideo.hpp"

namespace xeus_octave::widgets {

namespace xwidget {

inline octave_value_list display(const octave_value_list& args, int) {
	get_widget<xw::xcommon>(args(0).classdef_object_value())->display();
	return ovl();
}

inline octave_value_list id(const octave_value_list& args, int) {
	return ovl(ov<xeus::xguid>::to(get_widget<xw::xcommon>(args(0).classdef_object_value())->id()));
}

void register_xwidget(octave::interpreter& interpreter) {
	octave::cdef_manager& cm = interpreter.get_cdef_manager();

	// Build the class type
	octave::cdef_class cls = cm.make_class(XWIDGETS_BASE_CLASS_NAME);

	// Add xcommon methods
	xwidgets_add_method(interpreter, cls, "display", display);
	xwidgets_add_method(interpreter, cls, "id", id);
}

}  // namespace xwidget

void register_all(octave::interpreter& interpreter) {
	xwidget::register_xwidget(interpreter);

	xslider::register_all(interpreter);
	xbutton::register_all(interpreter);
	ximage::register_all(interpreter);
	xbox::register_all(interpreter);
	xoutput::register_all(interpreter);
	xtab::register_all(interpreter);
	xplay::register_all(interpreter);
	xaccordion::register_all(interpreter);
	xaudio::register_all(interpreter);
	xnumeral::register_all(interpreter);
	xprogress::register_all(interpreter);
	xvideo::register_all(interpreter);
}

}  // namespace xeus_octave::widgets