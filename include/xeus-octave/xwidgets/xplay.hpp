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

#ifndef XEUS_OCTAVE_XPLAY_H
#define XEUS_OCTAVE_XPLAY_H

#include "xeus-octave/xwidgets.hpp"

#include <xwidgets/xplay.hpp>

namespace xeus_octave::widgets
{

template <typename D> struct widget<xw::xplay<D>>
{
  using W = xw::xplay<D>;

  static constexpr auto name = "xplay";

  inline static void register_widget_members(octave::interpreter& interpreter, octave::cdef_class& cls)
  {
    static constexpr char const interval_name[] = "Interval";
    static constexpr char const step_name[] = "Step";
    static constexpr char const disabled_name[] = "Disabled";
    static constexpr char const playing_name[] = "Playing";
    static constexpr char const repeat_name[] = "Repeat";
    static constexpr char const show_repeat_name[] = "ShowRepeat";

    xwidgets_add_property<&W::interval, interval_name>(interpreter, cls);
    xwidgets_add_property<&W::step, step_name>(interpreter, cls);
    xwidgets_add_property<&W::disabled, disabled_name>(interpreter, cls);
    xwidgets_add_property<&W::_playing, playing_name>(interpreter, cls);
    xwidgets_add_property<&W::_repeat, repeat_name>(interpreter, cls);
    xwidgets_add_property<&W::show_repeat, show_repeat_name>(interpreter, cls);
  }
};

}  // namespace xeus_octave::widgets

#endif
