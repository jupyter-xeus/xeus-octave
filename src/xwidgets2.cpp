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
#include <ostream>
#include <string>

#include <nlohmann/json.hpp>
#include <octave/cdef-class.h>
#include <octave/cdef-manager.h>
#include <octave/cdef-object.h>
#include <octave/cdef-property.h>
#include <octave/cdef-utils.h>
#include <octave/error.h>
#include <octave/ov-base.h>
#include <octave/ov-classdef.h>
#include <octave/ov.h>
#include <octave/ovl.h>
#include <octave/parse.h>
#include <xwidgets/xcommon.hpp>

#include "xeus-octave/json.hpp"
#include "xeus-octave/utils.hpp"
#include "xeus-octave/xwidgets2.hpp"

namespace xw
{

}  // namespace xw

namespace xeus_octave::widgets
{

xwidget::xwidget() : octave::handle_cdef_object(), xw::xcommon()
{
  this->comm().on_message(std::bind(&xwidget::handle_message, this, std::placeholders::_1));
}

xwidget::~xwidget()
{
  std::clog << "Destructing " << get_class().get_name() << std::endl;
  this->close();
}

void xwidget::open()
{
  // serialize state
  nl::json state;
  xeus::buffer_sequence buffers;
  this->serialize_state(state, buffers);

  // open comm
  xw::xcommon::open(std::move(state), std::move(buffers));
}

void xwidget::close()
{
  xw::xcommon::close();
}

namespace
{

/**
 * @brief Check if property should be synced with widget model in frontend
 * by looking for "Sync" attribute"
 *
 * The following must be present in classdef definition in .m file
 *
 * ...
 *   properties (Sync = true)
 *     _model_name = "ButtonModel";
 *     _view_name = "ButtonView";
 *
 *     description = "";
 *     tooltip = "";
 *   end
 * ...
 *
 * We can use a nonstandard attribute because Octave parses all attributes
 * of properties regardless of their "correctness".
 *
 * @param property reference to a property definition object
 * @return true if property has attribute "Sync" set to true
 */
inline bool is_sync_property(octave::cdef_property& property)
{
  return !property.get("Sync").isempty() && property.get("Sync").bool_value();
}

};  // namespace

void xwidget::serialize_state(nl::json& state, xeus::buffer_sequence& buffers) const
{
  octave::cdef_class cls = this->get_class();
  auto properties = cls.get_property_map(octave::cdef_class::property_all);

  for (auto property_tuple : properties)
  {
    octave::cdef_property property = property_tuple.second;
    if (is_sync_property(property))
    {
      octave_value ov = this->get(property_tuple.first);
      xw::xwidgets_serialize(ov, state[property_tuple.first], buffers);
    }
  }
}

void xwidget::apply_patch(nl::json const& state, xeus::buffer_sequence const& buffers)
{
  octave::cdef_class cls = this->get_class();
  auto properties = cls.get_property_map(octave::cdef_class::property_all);

  for (auto property_tuple : properties)
  {
    octave::cdef_property property = property_tuple.second;
    if (properties.count(property_tuple.first) && is_sync_property(property) && state.contains(property_tuple.first))
    {
      octave_value value;
      xw::xwidgets_deserialize(value, state[property_tuple.first], buffers);
      // Call superclass put to avoid notifying the view again in a loop
      octave::handle_cdef_object::put(property_tuple.first, value);
      this->notify_backend(property_tuple.first);
    }
  }
}

void xwidget::put(std::string const& pname, octave_value const& val)
{
  octave::handle_cdef_object::put(pname, val);
  if (this->is_constructed())  // When default property values are being set
  {
    octave::cdef_class cls = this->get_class();
    auto properties = cls.get_property_map(octave::cdef_class::property_all);

    if (properties.count(pname) && is_sync_property(properties[pname]))
    {
      std::clog << "Notify change " << pname << std::endl;
      this->notify_frontend(pname, val);
      this->notify_backend(pname);
    }
  }
}

void xwidget::notify_frontend(std::string const& name, octave_value const& value)
{
  nl::json state;
  xeus::buffer_sequence buffers;
  xw::xwidgets_serialize(value, state[name], buffers);
  send_patch(std::move(state), std::move(buffers));
}

void xwidget::notify_backend(std::string const& pname) const
{
  // Get the observer property name
  std::string oname = "__" + pname + "_observers__";
  // Get the current list of handles
  octave_value fcn_list_ov = this->get(oname);
  // Call the observers
  if (!fcn_list_ov.isempty())
  {
    Cell fcn_list = fcn_list_ov.cell_value();
    for (octave_idx_type i = 0; i < fcn_list.numel(); i++)
    {
      // Object reference
      octave::cdef_object obj(this->clone());
      octave::feval(fcn_list(i), octave::to_ov(obj));
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

octave_value_list xwidget::cdef_constructor(octave_value_list const& args, int)
{
  // Get a reference to the old object
  octave::cdef_object& obj = args(0).classdef_object_value()->get_object_ref();
  // Retrieve the class we want to construct
  octave::cdef_class cls = obj.get_class();

  if (get_widget(args(0).classdef_object_value()) == nullptr)
  {
    std::clog << "Inject xwidget into " << cls.get_name() << std::endl;

    // Create a new object with our widget rep
    xwidget* wdg = new xwidget();
    octave::cdef_object new_obj(wdg);
    // Set it to the new object
    new_obj.set_class(cls);
    // Initialize the properties
    cls.initialize_object(new_obj);
    // Replace the old object
    obj = new_obj;
    // Open the comm
    wdg->open();

    return ovl(octave::to_ov(new_obj));
  }
  else  // If the object rep has already been substituted with an xwidget (this will happen with multiple inheritance)
  {
    std::clog << "No need to inject xwidget into " << cls.get_name() << std::endl;

    return ovl(args(0));
  }
}

octave_value_list xwidget::cdef_observe(octave_value_list const& args, int)
{
  // Object reference
  octave_classdef* obj = args(0).classdef_object_value();
  // Property to observe
  std::string pname = args(1).xstring_value("PNAME must be a string with the property name");
  std::string oname = "__" + pname + "_observers__";
  // Observer callback
  octave_value fcn = args(2);
  if (!fcn.is_function_handle())
    error("HANDLE must be a function handle");

  // Get the current list of handles
  octave_value fcn_list_ov = obj->get_object_ref().get(oname);

  std::clog << "Is empty: " << fcn_list_ov.isempty() << std::endl;

  // Append the function handle
  if (fcn_list_ov.isempty())
    obj->get_object_ref().put(oname, Cell(fcn));
  else
  {
    Cell fcn_list = obj->get_object_ref().get(oname).cell_value();
    fcn_list.resize1(fcn_list.numel() + 1);
    fcn_list(fcn_list.numel()) = fcn;
    obj->get_object_ref().put(oname, fcn_list);
  }

  return ovl();
}

octave_value_list xwidget::cdef_display(octave_value_list const& args, int)
{
  get_widget(args(0).classdef_object_value())->display();
  return ovl();
}

octave_value_list xwidget::cdef_id(octave_value_list const& args, int)
{
  return ovl(std::string(get_widget(args(0).classdef_object_value())->id()));
}

xwidget* get_widget(octave_classdef const* obj)
{
  octave::cdef_object const& ref = const_cast<octave_classdef*>(obj)->get_object_ref();
  octave::cdef_object_rep* rep = const_cast<octave::cdef_object_rep*>(ref.get_rep());

  return dynamic_cast<xwidget*>(rep);
}

void register_all2(octave::interpreter& interpreter)
{
  octave::cdef_manager& cm = interpreter.get_cdef_manager();
  octave::cdef_class cls = cm.make_class(XWIDGET_CLASS_NAME, cm.find_class("handle"));

  cls.install_method(cm.make_method(cls, XWIDGET_CLASS_NAME, xwidget::cdef_constructor));
  cls.install_method(cm.make_method(cls, "observe", xwidget::cdef_observe));
  cls.install_method(cm.make_method(cls, "display", xwidget::cdef_display));
  cls.install_method(cm.make_method(cls, "id", xwidget::cdef_id));

  interpreter.get_symbol_table().install_built_in_function(XWIDGET_CLASS_NAME, cls.get_constructor_function());
}

}  // namespace xeus_octave::widgets
