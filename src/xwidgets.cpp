/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#include "xeus-octave/xwidgets.hpp"

#include <octave/interpreter.h>
#include <octave/ov-classdef.h>

#include <iomanip>
#include <iostream>
#include <typeinfo>

#include "xwidgets/xslider.hpp"
#include "xwidgets/xtext.hpp"

#define XWIDGETS_POINTER_PROPERTY "__pointer__"

#define XWIDGETS_MAKE_CLASS(i, klass, widget)                                                                              \
	{                                                                                                                      \
		using widget_type = widget;                                                                                        \
		const std::string widget_name = #klass;                                                                            \
		octave::interpreter& widget_interp = i;                                                                            \
		octave::cdef_class widget_class = widget_interp.get_cdef_manager().make_class(widget_name);                        \
                                                                                                                           \
		octave_builtin::fcn constructor = [](const octave_value_list& args, int) {                                         \
			set_widget(args(0).classdef_object_value(), new widget_type);                                                  \
			return args;                                                                                                   \
		};                                                                                                                 \
		octave_builtin::fcn display = [](const octave_value_list& args, int) {                                             \
			get_widget<widget_type>(args(0).classdef_object_value())->display();                                           \
			return ovl();                                                                                                  \
		};                                                                                                                 \
		octave_builtin::fcn destructor = [](const octave_value_list& args, int) {                                          \
			delete get_widget<widget_type>(args(0).classdef_object_value());                                               \
			return ovl();                                                                                                  \
		};                                                                                                                 \
		widget_class.install_method(widget_interp.get_cdef_manager().make_method(widget_class, widget_name, constructor)); \
		widget_class.install_method(widget_interp.get_cdef_manager().make_method(widget_class, "display", display));       \
		widget_class.install_method(widget_interp.get_cdef_manager().make_method(widget_class, "delete", destructor));     \
		widget_interp.get_symbol_table().install_built_in_function(widget_name, widget_class.get_constructor_function());

#define XWIDGETS_MAKE_CLASS_END() \
	}

#define XWIDGETS_MAKE_PROPERTY(property)                                                                                                                                       \
	{                                                                                                                                                                          \
		octave_fcn_handle* get_##property##_h = new octave_fcn_handle(new octave_builtin(get_property<&widget_type::property, widget_type>, widget_name + ">get." #property)); \
		octave_fcn_handle* set_##property##_h = new octave_fcn_handle(new octave_builtin(set_property<&widget_type::property, widget_type>, widget_name + ">set." #property)); \
		widget_class.install_property(widget_interp.get_cdef_manager().make_property(                                                                                          \
			widget_class, #property,                                                                                                                                           \
			get_##property##_h, "public",                                                                                                                                      \
			set_##property##_h, "public"));                                                                                                                                    \
	}

namespace {

template <class W>
inline W* get_widget(octave_classdef* cls) {
	return reinterpret_cast<W*>(cls->get_property(0, XWIDGETS_POINTER_PROPERTY).long_value());
}

template <class W>
inline void set_widget(octave_classdef* cls, W* wdg) {
	cls->set_property(0, XWIDGETS_POINTER_PROPERTY, reinterpret_cast<intptr_t>(wdg));
}

template <class V, class T>
inline void set_property_from_ov(xp::xproperty<V, T>& property, octave_value value) {
	if constexpr (std::is_same_v<V, double>)
		property = value.double_value();
	else if constexpr (std::is_same_v<V, std::string>)
		property = value.string_value();
	else
		std::cerr << "Type " << typeid(V).name() << " is not handled" << std::endl;
}

template <auto property, class W>
inline octave_value_list set_property(const octave_value_list& args, int) {
	set_property_from_ov(get_widget<W>(args(0).classdef_object_value())->*property, args(1));
	return ovl();
}

template <auto property, class W>
inline octave_value_list get_property(const octave_value_list& args, int) {
	return ovl((get_widget<W>(args(0).classdef_object_value())->*property)());
}

void register_slider(octave::interpreter& interpreter) {
	XWIDGETS_MAKE_CLASS(interpreter, slider, xw::slider<double>)
	XWIDGETS_MAKE_PROPERTY(value)
	XWIDGETS_MAKE_PROPERTY(readout_format)
	XWIDGETS_MAKE_CLASS_END()

	octave_builtin::fcn a = set_property<&xw::text::disabled, xw::text>;
}

}  // namespace

namespace xeus_octave::widgets {

void register_all(octave::interpreter& interpreter) {
	register_slider(interpreter);
}

}  // namespace xeus_octave::widgets