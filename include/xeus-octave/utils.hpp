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

#ifndef XEUS_OCTAVE_UTILS_H
#define XEUS_OCTAVE_UTILS_H

#include <octave/dim-vector.h>
#include <octave/graphics-handle.h>
#include <octave/graphics.h>
#include <octave/int8NDArray.h>
#include <octave/interpreter.h>
#include <octave/ov-builtin.h>
#include <octave/ov-classdef.h>
#include <octave/ov-fcn-handle.h>
#include <octave/ov-null-mat.h>
#include <octave/ov.h>

#include <iostream>

#include <xeus/xguid.hpp>
#include <xtl/xoptional.hpp>
#include <xwidgets/xholder.hpp>
#include <xwidgets/ximage.hpp>

#include "xeus-octave/plotstream.hpp"

namespace xeus_octave::utils
{

/**
 * Convert octave_value to any generic type. Fails at runtime if not implemented in
 * octave default conversions
 */
template <class T> inline void from_ov(octave_value const&, T&, octave::interpreter&)
{
  static_assert(!sizeof(T*), "Please implement a proper conversion function");
}

/**
 * Convert octave_value to int
 */
inline void from_ov(octave_value const& from, int& to, octave::interpreter&)
{
  to = from.int_value();
}

/**
 * Convert octave_value to bool
 */
inline void from_ov(octave_value const& from, bool& to, octave::interpreter&)
{
  to = from.bool_value();
}

/**
 * Convert octave_value to std::string
 */
inline void from_ov(octave_value const& from, std::string& to, octave::interpreter&)
{
  to = from.string_value();
}

/**
 * Convert octave_value to double
 */
inline void from_ov(octave_value const& from, double& to, octave::interpreter&)
{
  to = from.scalar_value();
}

/**
 * Convert octave_value to xguid.
 * Used for xwidgets
 */
inline void from_ov(octave_value const& from, xeus::xguid& to, octave::interpreter& interpreter)
{
  std::string _to;
  from_ov(from, _to, interpreter);
  to = xeus::xguid(_to);
}

/**
 * Convert octave_value to widget (holder)
 */
inline void from_ov(octave_value const& from, xw::xholder& to, octave::interpreter& interpreter)
{
  // This is an xwidget
  if (from.is_classdef_object())
  {
    octave_classdef* cls = from.classdef_object_value();
    octave_value_list ret = cls->subsref(".", {ovl("id")}, 1);
    xeus::xguid id;

    from_ov(ret(0), id, interpreter);
    to = xw::make_id_holder(id);
  }
  // This is a figure handle
  else if (from.is_real_scalar())
  {
    // Get the handle
    graphics_handle gh = interpreter.get_gh_manager().lookup(from);

    if (gh.ok())
    {
      // Get the object
      octave::graphics_object go = interpreter.get_gh_manager().get_object(gh);
      // Change the graphics_toolkit to widget
      auto& figureProperties = dynamic_cast<octave::figure::properties&>(go.get_properties());
      figureProperties.set___graphics_toolkit__("__widget");
      auto* figure = getPlotStream<xw::image*>(go);

      assert(figure != nullptr);

      to = xw::make_id_holder(figure->id());
    }
  }
}

/**
 * Convert octave value to a generic vector
 */
template <class T> inline void from_ov(octave_value const& from, std::vector<T>& to, octave::interpreter& interpreter)
{
  Cell cell = from.cell_value();

  for (int i = 0; i < cell.numel(); i++)
  {
    T element;
    from_ov(cell(0, i), element, interpreter);
    to.push_back(element);
  }
}

/**
 * Convert octave_value to a vector of chars (i.e. byte array)
 */
inline void from_ov(octave_value const& from, std::vector<char>& to, octave::interpreter&)
{
  if (from.is_string())
  {
    std::string _to = from.string_value();
    std::copy(_to.begin(), _to.end(), std::back_inserter(to));
  }
  else
  {
    auto array = from.uint8_array_value();
    for (int i = 0; i < array.numel(); i++)
      to.push_back(array.xelem(i).char_value());
  }
}

/**
 * Convert octave_value to a xtl::xoptional generic value
 */
template <class T>
inline void from_ov(octave_value const& from, xtl::xoptional<T>& to, octave::interpreter& interpreter)
{
  if (!from.isnull())
  {
    T _to;
    from_ov(from, _to, interpreter);
    to = _to;
  }
  else
    to = xtl::xoptional<T>();
}

/**
 * Convert generic type to octave_value
 */
template <class T> inline void to_ov(octave_value& to, T const& from, octave::interpreter&)
{
  to = octave_value(from);
}

/**
 * Convert widget to octave_value.
 * Simply returns string containing id
 */
inline void to_ov(octave_value& to, xw::xholder const& from, octave::interpreter&)
{
  to = octave_value(from.id().c_str());
}

/**
 * Convert a generic std::vector to octave_value (cell array)
 */
template <class T> inline void to_ov(octave_value& to, std::vector<T> const& from, octave::interpreter& interpreter)
{
  Cell a;
  octave_idx_type i = 0;

  a.resize(dim_vector(static_cast<octave_idx_type>(from.size()), 1));

  for (auto e : from)
    to_ov(a.elem(i++, 0), e, interpreter);

  to = a;
}

/**
 * Convert a vector of char (i.e. bytearray) to octave_value (uint8 matrix)
 */
inline void to_ov(octave_value& to, std::vector<char> const& from, octave::interpreter&)
{
  auto dv = dim_vector(1, static_cast<octave_idx_type>(from.size()));
  uint8NDArray _to(Array<char>(from, dv));
  to = _to;
}

/**
 * Convert a generic xtl::xoptional to octave_value (value or null matrix)
 */
template <class T> inline void to_ov(octave_value& to, xtl::xoptional<T> const& from, octave::interpreter& interpreter)
{
  if (from.has_value())
    to_ov(to, from.value(), interpreter);
  else
    to = octave_null_matrix::instance;
}

/**
 * Convert a xguid to octave_value (sq_string)
 */
inline void to_ov(octave_value& to, xeus::xguid const& from, octave::interpreter& interpreter)
{
  to_ov(to, std::string(from), interpreter);
}

template <typename M> inline octave_value make_fcn_handle(M ff, std::string const& nm)
{
  octave_value fcn(new octave_builtin(ff, nm));
  return octave_value(new octave_fcn_handle(fcn));
}

inline void add_native_binding(octave::interpreter& interpreter, std::string const& name, octave_builtin::fcn ff)
{
  octave_builtin* fcn = new octave_builtin(ff, name, __FILE__, "");
  interpreter.get_symbol_table().install_built_in_function(name, fcn);
}

}  // namespace xeus_octave::utils

#endif
