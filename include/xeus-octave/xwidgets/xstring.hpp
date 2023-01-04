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

#ifndef XEUS_OCTAVE_XSTRING_H
#define XEUS_OCTAVE_XSTRING_H

#include "xeus-octave/xwidgets.hpp"

#include <xwidgets/xstring.hpp>

namespace xeus_octave::widgets
{

template <typename D> struct widget<xw::xstring<D>>
{
  using W = xw::xstring<D>;

  static constexpr auto name = "xstring";

  inline static void register_widget_members(octave::interpreter& interpreter, octave::cdef_class& cls)
  {
    static constexpr char const description_name[] = "Description";
    static constexpr char const value_name[] = "Value";
    static constexpr char const placeholder_name[] = "Placeholder";

    xwidgets_add_property<&W::description, description_name>(interpreter, cls);
    xwidgets_add_property<&W::value, value_name>(interpreter, cls);
    xwidgets_add_property<&W::placeholder, placeholder_name>(interpreter, cls);
  }
};

}  // namespace xeus_octave::widgets

#endif
