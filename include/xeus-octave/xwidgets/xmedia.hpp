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

#ifndef XEUS_OCTAVE_XMEDIA_H
#define XEUS_OCTAVE_XMEDIA_H

#include "xeus-octave/xwidgets.hpp"

#include <xwidgets/xmedia.hpp>

namespace xeus_octave::widgets
{

template <typename D> struct widget<xw::xmedia<D>>
{
  using W = xw::xmedia<D>;

  static constexpr auto name = "xmedia";

  inline static void register_widget_members(octave::interpreter& interpreter, octave::cdef_class& cls)
  {
    static constexpr char const value_name[] = "Value";

    xwidgets_add_property<&W::value, value_name>(interpreter, cls);
  };
};

}  // namespace xeus_octave::widgets

#endif
