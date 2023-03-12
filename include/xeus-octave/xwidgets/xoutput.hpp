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

#ifndef XEUS_OCTAVE_XOUTPUT_H
#define XEUS_OCTAVE_XOUTPUT_H

#include "xeus-octave/xwidgets.hpp"

#include <xwidgets/xoutput.hpp>

namespace xeus_octave::widgets
{

template <typename D> struct widget<xw::xoutput<D>>
{
  using W = xw::xoutput<D>;

  static constexpr auto name = "xoutput";

  inline static void register_widget_members(octave::interpreter& interpreter, octave::cdef_class& cls)
  {
    static constexpr char const msg_id_name[] = "MsgId";

    xwidgets_add_property<&W::msg_id, msg_id_name>(interpreter, cls);

    xwidgets_add_method(interpreter, cls, "capture", capture);
    xwidgets_add_method(interpreter, cls, "release", release);
  }

  inline static octave_value_list capture(octave_value_list const& args, int)
  {
    get_widget<W>(args(0).classdef_object_value())->capture();
    return ovl();
  }

  inline static octave_value_list release(octave_value_list const& args, int)
  {
    get_widget<W>(args(0).classdef_object_value())->release();
    return ovl();
  }
};

}  // namespace xeus_octave::widgets

#endif
