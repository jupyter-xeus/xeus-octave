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

#ifndef XEUS_OCTAVE_XBUTTON_H
#define XEUS_OCTAVE_XBUTTON_H

#include "xeus-octave/xwidgets.hpp"

#include <xwidgets/xbutton.hpp>

namespace xeus_octave::widgets
{

template <typename D> struct widget<xw::xbutton<D>>
{
  using W = xw::xbutton<D>;

  static constexpr auto name = "xbutton";

  inline static void register_widget_members(octave::interpreter& interpreter, octave::cdef_class& cls)
  {
    static constexpr char const description_name[] = "Description";
    static constexpr char const tooltip_name[] = "Tooltip";
    static constexpr char const disabled_name[] = "Disabled";
    static constexpr char const icon_name[] = "Icon";
    static constexpr char const button_style_name[] = "ButtonStyle";

    xwidgets_add_property<&W::description, description_name>(interpreter, cls);
    xwidgets_add_property<&W::tooltip, tooltip_name>(interpreter, cls);
    xwidgets_add_property<&W::disabled, disabled_name>(interpreter, cls);
    xwidgets_add_property<&W::icon, icon_name>(interpreter, cls);
    xwidgets_add_property<&W::button_style, button_style_name>(interpreter, cls);

    xwidgets_add_callback<&W::on_click>(interpreter, cls, "on_click");
  }
};

}  // namespace xeus_octave::widgets

#endif
