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

#ifndef XEUS_OCTAVE_XWIDGETS2_H
#define XEUS_OCTAVE_XWIDGETS2_H

#include <octave/cdef-object.h>
#include <octave/interpreter.h>
#include <octave/ov-base.h>
#include <octave/ov-classdef.h>
#include <octave/ov.h>
#include <octave/ovl.h>

#include <xwidgets/xcommon.hpp>

namespace xeus_octave::widgets
{

constexpr inline char const* XWIDGET_CLASS_NAME = "__xwidget_internal__";

class xwidget : public xw::xcommon
{
public:

  using base_type = xcommon;
  using base_type::notify;

private:

  xwidget(octave::interpreter&, octave::cdef_object_rep*);
  ~xwidget();

  void open();
  void close();

  void serialize_state(nl::json&, xeus::buffer_sequence&) const;
  void apply_patch(nl::json const&, xeus::buffer_sequence const&);
  void handle_message(xeus::xmessage const&);

public:

  static void register_all(octave::interpreter&);
  static xwidget* get_widget(octave_classdef const*);

private:

  static octave_value_list cdef_constructor(octave::interpreter& interpreter, octave_value_list const&, int);
  static octave_value_list cdef_destructor(octave_value_list const&, int);
  static octave_value_list cdef_display(octave_value_list const&, int);
  static octave_value_list cdef_subsasgn(octave::interpreter&, octave_value_list const&, int);
  static octave_value_list cdef_id(octave::interpreter&, octave_value_list const&, int);

  static void set_widget(octave_classdef* cls, xwidget const*);

  template <typename T> friend inline void xw::xwidgets_serialize(T const& value, nl::json& j, xeus::buffer_sequence&);

private:

  octave::cdef_object_rep* m_obj;
  octave::interpreter& m_interpreter;
};

}  // namespace xeus_octave::widgets
#endif