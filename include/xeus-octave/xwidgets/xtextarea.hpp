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

#ifndef XEUS_OCTAVE_XTEXTAREA_H
#define XEUS_OCTAVE_XTEXTAREA_H

#include "xeus-octave/xwidgets.hpp"

#include <xwidgets/xtextarea.hpp>

namespace xeus_octave::widgets
{

template <typename D> struct widget<xw::xtextarea<D>>
{
  using W = xw::xtextarea<D>;

  static constexpr auto name = "xtextarea";

  inline static void register_widget_members(octave::interpreter& interpreter, octave::cdef_class& cls)
  {
    static constexpr char const rows_name[] = "Rows";
    static constexpr char const disabled_name[] = "Disabled";
    static constexpr char const continuous_update_name[] = "ContinuousUpdate";

    xwidgets_add_property<&W::rows, rows_name>(interpreter, cls);
    xwidgets_add_property<&W::disabled, disabled_name>(interpreter, cls);
    xwidgets_add_property<&W::continuous_update, continuous_update_name>(interpreter, cls);
  }
};
}  // namespace xeus_octave::widgets

#endif
