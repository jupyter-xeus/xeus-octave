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

#include "xeus-octave/xwidgets2.hpp"

namespace nl = nlohmann;

namespace
{
template <typename M> void matrix_to_json(nl::json& j, M const& mv)
{
  j = nl::json::array();

  for (octave_idx_type i = 0; i < mv.numel(); i++)
    j.push_back(mv.elem(i));
}
}  // namespace

inline void to_json(nl::json&, octave_value const&);

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
    j = "IPY_MODEL_" + std::string(xeus_octave::widgets::xwidget::get_widget(&cdv)->id());
  else
    warning("xwidget: cannot serialize classdef");
}

inline void to_json(nl::json& j, octave_value const& ov)
{
  if (ov.is_real_scalar())
    j = ov.scalar_value();
  else if (ov.is_bool_scalar())
    j = ov.bool_value();
  else if (ov.is_string())
    j = ov.string_value();
  else if (ov.is_classdef_object())
    j = *ov.classdef_object_value();
  else if (ov.is_real_matrix())
    j = ov.matrix_value();
  else if (ov.is_bool_matrix())
    j = ov.bool_matrix_value();
  else if (ov.isstruct())
    j = ov.scalar_map_value();
  else if (ov.iscell())
    j = ov.cell_value();
  else if (ov.isnull())
    j = nullptr;
  else
    warning("xwidget: cannot serialize octave value");
}

inline void from_json(nl::json const& j, octave_value& p)
{
  if (j.is_number())
    p = octave_value(static_cast<double>(j));
  else
    warning("xwidget: cannot deserialize frontend message");
}

#endif
