/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XVIDEO_HPP
#define XEUS_OCTAVE_XVIDEO_HPP

#include "xeus-octave/xwidgets.hpp"
#include "xeus-octave/xwidgets/xmedia.hpp"
#include "xwidgets/xvideo.hpp"

namespace xeus_octave::widgets::xvideo {

using namespace xeus_octave::widgets;
using namespace xeus_octave::widgets::xmedia;

inline void register_all(octave::interpreter& interpreter) {
	using W = xw::video;

	octave::cdef_manager& cm = interpreter.get_cdef_manager();
	octave::cdef_class cls = xwidgets_make_class<W>(interpreter, "xvideo");

	xwidgets_inherit_xmedia<W>(interpreter, cls);

	xwidgets_add_property<W, &W::format>(interpreter, cls, "Format");
	xwidgets_add_property<W, &W::width>(interpreter, cls, "Width");
	xwidgets_add_property<W, &W::height>(interpreter, cls, "Height");
	xwidgets_add_property<W, &W::autoplay>(interpreter, cls, "Autoplay");
	xwidgets_add_property<W, &W::loop>(interpreter, cls, "Loop");
	xwidgets_add_property<W, &W::controls>(interpreter, cls, "Controls");
}

}  // namespace xeus_octave::widgets::xaudio

#endif