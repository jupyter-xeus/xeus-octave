/*
 * Copyright (C) 2022 Giulio Girardi.
 *
 * This file is part of xeus-octave.
 *
 * xeus-octave is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * xeus-octave is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xeus-octave.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef XEUS_OCTAVE_XAUDIO_H
#define XEUS_OCTAVE_XAUDIO_H

#include "xeus-octave/xwidgets.hpp"

#include <xwidgets/xaudio.hpp>

namespace xeus_octave::widgets
{

template <typename D> struct widget<xw::xaudio<D>>
{
  using W = xw::xaudio<D>;

  static constexpr auto name = "xaudio";

  inline static void register_widget_members(octave::interpreter& interpreter, octave::cdef_class& cls)
  {
    static constexpr char const format_name[] = "Format";
    static constexpr char const autoplay_name[] = "Autoplay";
    static constexpr char const loop_name[] = "Loop";
    static constexpr char const controls_name[] = "Controls";

    xwidgets_add_property<&W::format, format_name>(interpreter, cls);
    xwidgets_add_property<&W::autoplay, autoplay_name>(interpreter, cls);
    xwidgets_add_property<&W::loop, loop_name>(interpreter, cls);
    xwidgets_add_property<&W::controls, controls_name>(interpreter, cls);
  }
};

}  // namespace xeus_octave::widgets

#endif
