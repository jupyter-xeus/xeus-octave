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

#ifndef XEUS_OCTAVE_XCOMMON_H
#define XEUS_OCTAVE_XCOMMON_H

#include "xeus-octave/xwidgets.hpp"

#include <octave/interpreter.h>
#include <xwidgets/xcommon.hpp>

namespace xeus_octave::widgets
{

template <> struct widget<xw::xcommon>
{
  using W = xw::xcommon;

  static constexpr auto name = "xcommon";

  inline static void register_widget_members(octave::interpreter& interpreter, octave::cdef_class& cls)
  {
    xwidgets_add_method(interpreter, cls, "display", display);
    xwidgets_add_method(interpreter, cls, "id", id);
  };

  inline static octave_value_list display(octave_value_list const& args, int)
  {
    get_widget<W>(args(0).classdef_object_value())->display();
    return ovl();
  }

  inline static octave_value_list id(octave::interpreter& interpreter, octave_value_list const& args, int)
  {
    using namespace utils;

    octave_value v;
    to_ov(v, get_widget<W>(args(0).classdef_object_value())->id(), interpreter);
    return v;
  }
};

}  // namespace xeus_octave::widgets

#endif
