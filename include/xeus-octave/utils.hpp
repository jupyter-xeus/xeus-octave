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

#ifndef XEUS_OCTAVE_UTILS_HPP
#define XEUS_OCTAVE_UTILS_HPP

#include <octave/interpreter.h>
#include <octave/ov-builtin.h>
#include <octave/ov-fcn-handle.h>
#include <octave/ov-null-mat.h>
#include <octave/ov.h>

#include <iostream>

#include <xeus/xguid.hpp>

namespace xeus_octave::utils
{

inline void add_native_binding(octave::interpreter& interpreter, std::string const& name, octave_builtin::fcn ff)
{
  octave_builtin* fcn = new octave_builtin(ff, name, __FILE__, "");
  interpreter.get_symbol_table().install_built_in_function(name, fcn);
}

}  // namespace xeus_octave::utils

#endif
