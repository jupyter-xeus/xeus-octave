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

#include <string>
#include <xwidgets/xcommon.hpp>

namespace xeus_octave::widgets
{

void register_all2(octave::interpreter&);

constexpr inline char const* XWIDGET_CLASS_NAME = "__xwidget_internal__";

class xwidget : public octave::handle_cdef_object, public xw::xcommon
{

public:

  void put(std::string const&, octave_value const&) override;
  void mark_as_constructed(octave::cdef_class const& cls) override;

private:

  xwidget();
  ~xwidget();

  void open();
  void close();

  void serialize_state(nl::json&, xeus::buffer_sequence&) const;
  void apply_patch(nl::json const&, xeus::buffer_sequence const&);
  void handle_message(xeus::xmessage const&);

  /**
   * @brief call any observers set in the octave interpreter context for the
   * specified property name
   */
  void notify_backend(std::string const&);
  /**
   * @brief send to the frontend a new value for the specified property.
   * Octave value is automatically converted to a json value
   */
  void notify_frontend(std::string const&, octave_value const&);

private:

  static octave_value_list cdef_constructor(octave::interpreter&, octave_value_list const&, int);
  static octave_value_list cdef_observe(octave_value_list const&, int);
  static octave_value_list cdef_display(octave_value_list const&, int);
  static octave_value_list cdef_id(octave_value_list const&, int);

  template <typename T> friend inline void xw::xwidgets_serialize(T const& value, nl::json& j, xeus::buffer_sequence&);
  friend void xeus_octave::widgets::register_all2(octave::interpreter&);

private:

  std::map<std::string, std::list<octave_value>> m_observerCallbacks;
};

xwidget* get_widget(octave_classdef const*);

}  // namespace xeus_octave::widgets
#endif
