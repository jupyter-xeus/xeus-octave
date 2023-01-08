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

#include <iostream>
#include <nlohmann/json_fwd.hpp>
#include <octave/cdef-class.h>
#include <octave/cdef-manager.h>
#include <octave/cdef-object.h>
#include <octave/cdef-property.h>
#include <octave/error.h>
#include <octave/ov-base.h>
#include <octave/ov-classdef.h>
#include <octave/ov.h>

#include <octave/ovl.h>
#include <xwidgets/xcommon.hpp>

#include "xeus-octave/json.hpp"
#include "xeus-octave/utils.hpp"
#include "xeus-octave/xwidgets2.hpp"

constexpr inline char const* XWIDGET_POINTER_PROPERTY = "__pointer__";

namespace xw
{

}  // namespace xw

namespace xeus_octave::widgets
{

xwidget::xwidget(octave::interpreter& interpreter, octave::cdef_object_rep* obj) :
  base_type(), m_obj(obj), m_interpreter(interpreter)
{
  this->comm().on_message(std::bind(&xwidget::handle_message, this, std::placeholders::_1));
  this->open();
}

xwidget::~xwidget()
{
  this->close();
}

void xwidget::open()
{
  // serialize state
  nl::json state;
  xeus::buffer_sequence buffers;
  this->serialize_state(state, buffers);

  // open comm
  base_type::open(std::move(state), std::move(buffers));
}

void xwidget::close()
{
  base_type::close();
}

void xwidget::serialize_state(nl::json& state, xeus::buffer_sequence& buffers) const
{
  octave::cdef_manager& cm = m_interpreter.get_cdef_manager();
  std::string class_name = m_obj->class_name();
  octave::cdef_class cls = cm.find_class(class_name);
  auto properties = cls.get_property_map(octave::cdef_class::property_all);

  for (auto property_tuple : properties)
  {
    octave::cdef_property property = std::get<1>(property_tuple);
    octave::cdef_object tmp(m_obj->clone());
    octave_value ov = property.get_value(tmp, true, "serialize_state");
    xw::xwidgets_serialize(ov, state[property.get_name()], buffers);
  }
}

void xwidget::apply_patch(nl::json const& state, xeus::buffer_sequence const&)
{
  octave::cdef_manager& cm = m_interpreter.get_cdef_manager();
  std::string class_name = m_obj->class_name();
  octave::cdef_class cls = cm.find_class(class_name);
  auto properties = cls.get_property_map(octave::cdef_class::property_all);

  for (auto property_tuple : properties)
  {
    octave::cdef_property property = std::get<1>(property_tuple);
    if (state.contains(property.get_name()))
    {
      octave::cdef_object tmp(m_obj->clone());
      property.set_value(tmp, state[property.get_name()], true, "apply_patch");
    }
  }
}

void xwidget::handle_message(xeus::xmessage const& message)
{
  nl::json const& content = message.content();
  nl::json const& data = content["data"];
  const std::string method = data["method"];

  if (method == "update")
  {
    nl::json const& state = data["state"];
    auto const& buffers = message.buffers();
    nl::json const& buffer_paths = data["buffer_paths"];
    this->hold() = std::addressof(message);
    xw::insert_buffer_paths(const_cast<nl::json&>(state), buffer_paths);
    this->apply_patch(state, buffers);
    this->hold() = nullptr;
  }
  else if (method == "request_state")
  {
    nl::json state;
    xeus::buffer_sequence buffers;
    this->serialize_state(state, buffers);
    send_patch(std::move(state), std::move(buffers));
  }
  else if (method == "custom")
  {
    auto it = data.find("content");
    if (it != data.end())
    {
      this->handle_custom_message(it.value());
    }
  }
}

void xwidget::register_all(octave::interpreter& interpreter)
{
  octave::cdef_manager& cm = interpreter.get_cdef_manager();
  octave::cdef_class cls = cm.make_class(XWIDGET_CLASS_NAME, cm.find_class("handle"));

  cls.install_method(cm.make_method(cls, XWIDGET_CLASS_NAME, xwidget::cdef_constructor));
  cls.install_method(cm.make_method(cls, "display", xwidget::cdef_display));
  cls.install_method(cm.make_method(cls, "id", xwidget::cdef_id));
  cls.install_method(cm.make_method(cls, "delete", xwidget::cdef_destructor));
  cls.install_method(cm.make_method(cls, "subsasgn", xwidget::cdef_subsasgn));

  interpreter.get_symbol_table().install_built_in_function(XWIDGET_CLASS_NAME, cls.get_constructor_function());
}

octave_value_list xwidget::cdef_constructor(octave::interpreter& interpreter, octave_value_list const& args, int)
{
  octave_classdef* obj = args(0).classdef_object_value();
  xwidget* widget = get_widget(obj);

  if (!widget)
  {
    octave::cdef_object const& object = args(0).classdef_object_value()->get_object_ref();
    octave::cdef_object_rep const* rep = object.get_rep();
    widget = new xwidget(interpreter, const_cast<octave::cdef_object_rep*>(rep));
  }

  set_widget(obj, widget);
  return args(0);
}

octave_value_list xwidget::cdef_destructor(octave_value_list const& args, int)
{
  octave_classdef* obj = args(0).classdef_object_value();
  xwidget* widget = get_widget(obj);
  set_widget(obj, nullptr);

  if (widget)
    delete widget;

  return ovl();
}

octave_value_list xwidget::cdef_display(octave_value_list const& args, int)
{
  get_widget(args(0).classdef_object_value())->display();
  return ovl();
}

octave_value_list xwidget::cdef_subsasgn(octave::interpreter& interpreter, octave_value_list const& args, int)
{
  octave_classdef* obj = args(0).classdef_object_value();
  octave_map map = args(1).map_value();
  octave_value rhs = args(2);

  Cell type = map.getfield("type");
  Cell subs = map.getfield("subs");

  if (type.numel() == 1 && type(0).string_value() == ".")
  {
    octave::cdef_manager& cm = interpreter.get_cdef_manager();
    std::string class_name = obj->class_name();
    octave::cdef_class cls = cm.find_class(class_name);
    auto properties = cls.get_property_map(octave::cdef_class::property_all);
    auto widget = get_widget(obj);
    std::string property_name = subs(0).string_value();

    if (properties.count(property_name))
    {
      // Perform manually the subsasgn call
      octave::cdef_property property = properties[property_name];
      octave_value_list retval = obj->get_object_ref().subsasgn(".", {ovl(property_name)}, rhs);
      widget->notify(property_name, property.get_value(obj->get_object_ref(), false, "subsasgn"));
      return retval;
    }
  }

  // Return an undefined object to have Octave run the real subsasgn function
  // Add a fake additional return value to avoid having an empty return value
  return ovl(octave_value(), 0);
}

octave_value_list xwidget::cdef_id(octave::interpreter& interpreter, octave_value_list const& args, int)
{
  using namespace utils;

  octave_value v;
  to_ov(v, get_widget(args(0).classdef_object_value())->id(), interpreter);
  return v;
}

inline xwidget* xwidget::get_widget(octave_classdef const* obj)
{
  octave_value p = obj->get_object().get(XWIDGET_POINTER_PROPERTY);

  if (p.isempty())
    return nullptr;
  else
    return reinterpret_cast<xwidget*>(p.ulong_value());
}

inline void xwidget::set_widget(octave_classdef* obj, xwidget const* widget)
{

  obj->get_object_ref().put(XWIDGET_POINTER_PROPERTY, reinterpret_cast<uintptr_t>(widget));
}

}  // namespace xeus_octave::widgets