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

#ifndef XEUS_OCTAVE_XWIDGETS_H
#define XEUS_OCTAVE_XWIDGETS_H

#include <cstddef>
#include <string>
#include <type_traits>

#include <octave/cdef-class.h>
#include <octave/cdef-fwd.h>
#include <octave/cdef-manager.h>
#include <octave/interpreter.h>
#include <octave/ov-classdef.h>

#include <xproperty/xproperty.hpp>
#include <xwidgets/xcommon.hpp>
#include <xwidgets/xmaterialize.hpp>

#include "xeus-octave/utils.hpp"

namespace xeus_octave::widgets
{

constexpr inline char const* XWIDGETS_POINTER_PROPERTY = "__pointer__";

template <typename> struct xwidgets_member_trait;

template <typename V, typename W> struct xwidgets_member_trait<V W::*>
{
  using widget_type = W;
};

template <typename> struct widget
{
  inline static constexpr char const* name = nullptr;

  inline static void register_widget_members(octave::interpreter&, octave::cdef_class&){};
};

template <class W> inline octave::cdef_class xwidget_get_class(octave::cdef_manager& cm)

{
  using widget_type = W;

  // Class does not correspond to an octave cdef
  if constexpr (!std::is_same<widget_type, xw::xcommon>::value)
    if (widget<widget_type>::name == nullptr)
      return xwidget_get_class<typename widget_type::base_type>(cm);

  octave::cdef_class cls = cm.find_class(widget<widget_type>::name, false, false);

  // Class has already been created
  if (cls.ok())
    return cls;

  // Class needs to be created
  if constexpr (!std::is_same<widget_type, xw::xcommon>::value)
    return cm.make_class(widget<widget_type>::name, xwidget_get_class<typename widget_type::base_type>(cm));
  else
    return cm.make_class(widget<widget_type>::name, cm.find_class("handle"));
}

/**
 * Recursively call the functions to register all members of the cdef object
 * belonging to a specific class
 */
template <class W> inline void xwidget_register_members(octave::interpreter& interpreter, octave::cdef_class& cls)
{
  using widget_type = W;

  widget<widget_type>::register_widget_members(interpreter, cls);

  if constexpr (!std::is_same<widget_type, xw::xcommon>::value)
    xwidget_register_members<typename W::base_type>(interpreter, cls);
}

/**
 * Installs a method in the cdef object.
 */
template <typename M>
inline void xwidgets_add_method(octave::interpreter& interpreter, octave::cdef_class& cls, std::string name, M ff)
{
  octave::cdef_manager& cm = interpreter.get_cdef_manager();

  cls.install_method(cm.make_method(cls, name, ff));
}

/**
 * Retrieves from a cdef object the pointer to the xwidget class instance
 */
template <class W> inline W* get_widget(octave_classdef const* cls)
{
  return reinterpret_cast<W*>(cls->get_property(0, XWIDGETS_POINTER_PROPERTY).ulong_value());
}

/**
 * Stores in a cdef object a pointer to an xwidget class instance
 */
template <class W> inline void set_widget(octave_classdef* cls, W const* wdg)
{
  cls->set_property(0, XWIDGETS_POINTER_PROPERTY, reinterpret_cast<uintptr_t>(wdg));
}

/**
 * Octave cdef constructor, it simply instanciates the corresponding xwidget class
 * and stores the pointer in the cdef object.
 *
 * The xwidget class is instanciated dynamically
 */
template <class W> inline octave_value_list constructor(octave_value_list const& args, int)
{
  using widget_type = W;

  set_widget(args(0).classdef_object_value(), new widget_type);
  return args;
}

/**
 * Retrieves the xwidget instance from the cdef object and destroys it
 */
template <class W> inline octave_value_list destructor(octave_value_list const& args, int)
{
  using widget_type = W;

  delete get_widget<widget_type>(args(0).classdef_object_value());
  return ovl();
}

/**
 * Set an xproperty value from an octave_value, performing automatic conversion
 * under the assumption that the corresponding from_ov function is defined
 */
template <typename V, typename T>
inline void set_property(xp::xproperty<V, T>& property, octave_value const& value, octave::interpreter& interpreter)
{
  using namespace utils;
  using value_type = V;

  value_type pvalue;
  from_ov(value, pvalue, interpreter);
  property = pvalue;
}

/**
 * cdef object property setter function
 */
template <auto P, char const* N, bool C = false>
inline octave_value_list set_property(octave::interpreter& interpreter, octave_value_list const& args, int)
{
  using property_type = decltype(P);
  using widget_type = typename xwidgets_member_trait<property_type>::widget_type;

  std::string name(N);
  // Get cdef object
  octave_classdef* cls = args(0).classdef_object_value();
  // Retrieve xwidget instance
  widget_type* w = get_widget<widget_type>(cls);

  // If the variable is cached set a cdef "static" property as cache
  if constexpr (C)
    cls->set_property(0, name + "_cache", args(1));

  // Set the xwidget property
  set_property(w->*P, args(1), interpreter);
  return ovl();
}

/**
 * Get an octave value for an xproperty actual value, performing automatic conversion
 * under the assumption that the corresponding to_ function is defined
 */
template <class V, class T>
inline octave_value get_property(xp::xproperty<V, T>& property, octave::interpreter& interpreter)
{
  using namespace utils;

  octave_value value;
  to_ov(value, property(), interpreter);
  return value;
}

/**
 * cdef object property getter function
 */
template <auto P, char const* N, bool C = false>
inline octave_value_list get_property(octave::interpreter& interpreter, octave_value_list const& args, int)
{
  using property_type = decltype(P);
  using widget_type = typename xwidgets_member_trait<property_type>::widget_type;

  std::string name(N);
  // Get cdef object
  octave_classdef* cls = args(0).classdef_object_value();
  // Retrieve xwidget instance
  widget_type* w = get_widget<widget_type>(cls);

  // If the variable is cached return the cache value instead of converting the
  // actual xproperty value
  if constexpr (C)
    return cls->get_property(0, name + "_cache");
  else
    return get_property(w->*P, interpreter);
}

/**
 * cdef object observer function
 */
template <auto P> inline octave_value_list observe(octave::interpreter& interpreter, octave_value_list const& args, int)
{
  using widget_type = typename xwidgets_member_trait<decltype(P)>::widget_type;

  // Get cdef object
  octave_classdef* cls = args(0).classdef_object_value();
  // Retrieve xwidget instance
  widget_type* w = get_widget<widget_type>(cls);
  // Property instance
  auto p = w->*P;

  // Register the xwidget observer
  w->observe(
    p.name(),
    [cls = args(0),                            // cdef object
     cb = args(1),                             // Callback object
     args = args.slice(2, args.length() - 2),  // Other parameters
     &interpreter](auto const&)
    {
      octave_value_list cb_args;

      // Prepare the parameters passed to the callback
      cb_args(0) = cls;      // The cdef object
      cb_args.append(args);  // All the other parameters

      // Evaluate the callback
      interpreter.feval(cb, cb_args);
    }
  );

  return ovl();
}

/**
 * Cdef function to set a callback
 */
template <auto P>
inline octave_value_list set_callback(octave::interpreter& interpreter, octave_value_list const& args, int)
{
  using property_type = decltype(P);
  using widget_type = typename xwidgets_member_trait<property_type>::widget_type;

  // Get cdef object
  octave_classdef* cls = args(0).classdef_object_value();
  // Retrieve xwidget instance
  widget_type* w = get_widget<widget_type>(cls);

  (w->*P)(
    [cls = args(0),                            // cdef object
     cb = args(1),                             // Callback object
     args = args.slice(2, args.length() - 2),  // Other parameters
     &interpreter]()
    {
      octave_value_list cb_args;

      // Prepare the parameters passed to the callback
      cb_args(0) = cls;      // The cdef object
      cb_args.append(args);  // All the other parameters

      // Evaluate the callback
      interpreter.feval(cb, cb_args);
    }
  );

  return ovl();
}

/**
 * Add a function to register a callback to the cdef class
 */
template <auto P>
inline void xwidgets_add_callback(octave::interpreter& interpreter, octave::cdef_class& cls, std::string name)
{

  octave::cdef_manager& cm = interpreter.get_cdef_manager();

  // Add basic methods
  cls.install_method(cm.make_method(cls, name, set_callback<P>));
}

/**
 * Link a xwidget property with a cdef property
 */
template <auto P, char const* N, bool R = false, bool C = false>
inline void xwidgets_add_property(octave::interpreter& interpreter, octave::cdef_class& cls)
{
  using namespace xeus_octave::utils;
  using property_type = decltype(P);
  using widget_type = typename xwidgets_member_trait<property_type>::widget_type;

  octave::cdef_manager& cm = interpreter.get_cdef_manager();
  std::string name(N);

  // Readonly property
  if constexpr (R)
  {
    xwidgets_add_method(interpreter, cls, name, get_property<P, N>);
  }
  // Read-write property
  else
  {
    cls.install_property(cm.make_property(
      cls,
      name,
      make_fcn_handle(get_property<P, N, C>, widget<widget_type>::name + (">get." + name)),
      "public",
      make_fcn_handle(set_property<P, N, C>, widget<widget_type>::name + (">set." + name)),
      "public"
    ));

    cls.install_method(cm.make_method(cls, "observe_" + name, observe<P>));
  }
}

/**
 * Register an xwidget class. End users must call this function to perform the
 * registration
 */
template <template <class> class W, class... P> struct widget<xw::xmaterialize<W, P...>>
{
  inline static void register_widget(octave::interpreter& interpreter)
  {
    using base_type = xw::xmaterialize<W, P...>;
    using widget_type = W<base_type>;

    static_assert(widget<widget_type>::name != nullptr, "xwidget name must be defined");

    // Get octave cdef (and create it if necessary)
    octave::cdef_class cls = xwidget_get_class<widget_type>(interpreter.get_cdef_manager());

    // Register widgtetp properties and functions to cdef class
    xwidget_register_members<widget_type>(interpreter, cls);

    // Add constructor and destructor
    xwidgets_add_method(interpreter, cls, widget<widget_type>::name, constructor<base_type>);
    xwidgets_add_method(interpreter, cls, "delete", destructor<base_type>);

    // Register the constructor
    interpreter.get_symbol_table().install_built_in_function(widget<widget_type>::name, cls.get_constructor_function());
  }
};

void register_all(octave::interpreter& interpreter);

}  // namespace xeus_octave::widgets

#endif
