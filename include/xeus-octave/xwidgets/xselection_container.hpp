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

#ifndef XEUS_OCTAVE_XSELECTION_CONTAINER_H
#define XEUS_OCTAVE_XSELECTION_CONTAINER_H

#include "xeus-octave/xwidgets.hpp"

#include <xwidgets/xselection_container.hpp>

namespace xeus_octave::widgets
{

template <typename D> struct widget<xw::xselection_container<D>>
{
  using W = xw::xselection_container<D>;

  static constexpr auto name = "xselection_container";

  inline static void register_widget_members(octave::interpreter& interpreter, octave::cdef_class& cls)
  {
    static constexpr char const titles_name[] = "Titles";
    static constexpr char const selected_index_name[] = "SelectedIndex";

    xwidgets_add_property<&W::_titles, titles_name>(interpreter, cls);
    xwidgets_add_property<&W::selected_index, selected_index_name>(interpreter, cls);

    xwidgets_add_method(interpreter, cls, "set_title", set_title);
  }

  inline static octave_value_list set_title(octave_value_list const& args, int)
  {
    get_widget<W>(args(0).classdef_object_value())->set_title(args(1).ulong_value(), args(2).string_value());
    return ovl();
  }
};

}  // namespace xeus_octave::widgets

#endif
