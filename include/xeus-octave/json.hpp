/*
 * Copyright (C) 2023 Giulio Girardi.
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

#ifndef XEUS_OCTAVE_JSON_H
#define XEUS_OCTAVE_JSON_H

#include <nlohmann/json.hpp>

#include <octave/Cell.h>
#include <octave/boolMatrix.h>
#include <octave/error.h>
#include <octave/oct-map.h>
#include <octave/ov-classdef.h>
#include <octave/ov.h>
#include <string>
#include <utility>

#include "xeus-octave/xwidgets2.hpp"

namespace nl = nlohmann;

inline void to_json(nl::json&, octave_value const&);
inline void from_json(nl::json const& j, octave_value& p);

namespace
{

template <typename M> inline void matrix_to_json(nl::json& j, M const& mv)
{
  j = nl::json::array();

  for (octave_idx_type i = 0; i < mv.numel(); i++)
    j.push_back(mv.elem(i));
}
}  // namespace

/**
 * Octave to JSON type conversion:
 *
 * | Octave type           | JSON Type              | Notes          |
 * |-----------------------|------------------------|----------------|
 * | empty                 | null                   |                |
 * | real scalar           | number                 |                |
 * | bool scalar           | bool                   |                |
 * | string                | string                 |                |
 * | map (struct)          | obect                  |                |
 * | real matrix           | array of numbers       |                |
 * | cell array            | array of any           |                |
 * | cell array of widgets | array of "IPY_MODEL_*" | unidirectional |
 */

inline void to_json(nl::json& j, octave_scalar_map const& om)
{
  j = nl::json::object();

  for (auto p = om.begin(); p != om.end(); p++)
    j[om.key(p)] = om.contents(p);
}

inline void to_json(nl::json& j, Matrix const& mv)
{
  matrix_to_json(j, mv);
}

inline void to_json(nl::json& j, boolMatrix const& mv)
{
  matrix_to_json(j, mv);
}

inline void to_json(nl::json& j, Cell const& mv)
{
  matrix_to_json(j, mv);
}

inline void to_json(nl::json& j, octave_classdef const& cdv)
{
  if (cdv.is_instance_of(xeus_octave::widgets::XWIDGET_CLASS_NAME))
    j = "IPY_MODEL_" + std::string(xeus_octave::widgets::get_widget(&cdv)->id());
  else
    error("xwidget: cannot serialize classdef");
}

inline void to_json(nl::json& j, octave_value const& ov)
{
  if (ov.is_bool_scalar())
    j = ov.bool_value();
  else if (ov.is_real_scalar())
    j = ov.scalar_value();
  else if (ov.is_string())
    j = ov.string_value();
  else if (ov.isstruct())
    j = ov.scalar_map_value();
  else if (ov.isempty())
    j = nullptr;
  else if (ov.is_real_matrix())
    j = ov.matrix_value();
  else if (ov.is_bool_matrix())
    j = ov.bool_matrix_value();
  else if (ov.iscell())
    j = ov.cell_value();
  else if (ov.is_classdef_object())
    j = *ov.classdef_object_value();
  else
    error("xwidget: cannot serialize octave value");
}

namespace
{

inline octave_value from_json_string(nl::json const& j)
{
  std::string p = j.get<std::string>();

  if (p.rfind("IPY_MODEL_", 0) == 0)
    error("xwidget: cannot convert back a widget reference");

  return j;
}

inline octave_scalar_map from_json_object(nl::json const& j)
{
  octave_scalar_map p;

  for (auto& [key, val] : j.items())
    p.assign(key, val);

  return p;
}

template <typename T> inline octave_value from_json_array(nl::json const& j)
{
  T p(dim_vector(static_cast<octave_idx_type>(j.size()), 1));
  octave_idx_type i = 0;
  for (auto& e : j)
    p(i++) = e;
  return p;
}

inline octave_value from_json_array(nl::json const& j)
{
  if (j.empty())
    return Matrix();

  auto array_type = j[0].type();
  for (auto& e : j)
    if (e.type() != array_type)
    {
      array_type = nl::detail::value_t::discarded;
      break;
    }

  switch (array_type)
  {
  case nl::detail::value_t::number_float:
  case nl::detail::value_t::number_integer:
  case nl::detail::value_t::number_unsigned:
    return from_json_array<Matrix>(j);
  case nl::detail::value_t::boolean:
    return from_json_array<boolMatrix>(j);
  default:
    return from_json_array<Cell>(j);
  }
}

}  // namespace

inline void from_json(nl::json const& j, octave_value& p)
{
  if (j.is_boolean())
    p = octave_value(j.get<bool>());
  else if (j.is_number())
    p = octave_value(j.get<double>());
  else if (j.is_string())
    p = from_json_string(j);
  else if (j.is_object())
    p = from_json_object(j);
  else if (j.is_null())
    p = octave_value();
  else if (j.is_array())
    p = from_json_array(j);
  else
    error("xwidget: cannot deserialize frontend message");
}

#endif
