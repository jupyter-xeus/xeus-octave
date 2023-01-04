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

#ifndef XEUS_OCTAVE_XLABEL_H
#define XEUS_OCTAVE_XLABEL_H

#include "xeus-octave/xwidgets.hpp"

#include <xwidgets/xlabel.hpp>

namespace xeus_octave::widgets
{

template <typename D> struct widget<xw::xlabel<D>>
{
  using W = xw::xlabel<D>;

  static constexpr auto name = "xlabel";

  inline static void register_widget_members(octave::interpreter&, octave::cdef_class&) {}
};

}  // namespace xeus_octave::widgets

#endif
