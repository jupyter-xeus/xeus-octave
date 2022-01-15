/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XWIDGETS_HPP
#define XEUS_OCTAVE_XWIDGETS_HPP

#include <octave/interpreter.h>
#include <octave/ov-classdef.h>

#include "xeus-octave/utils.hpp"
#include "xproperty/xproperty.hpp"
#include "xwidgets/xcommon.hpp"

namespace xeus_octave::widgets {

using namespace xeus_octave::utils;

#define XWIDGETS_POINTER_PROPERTY "__pointer__"
#define XWIDGETS_BASE_CLASS_NAME "xwidget"

template <class W>
inline W* get_widget(const octave_classdef* cls) {
	return reinterpret_cast<W*>(cls->get_property(0, XWIDGETS_POINTER_PROPERTY).long_value());
}

template <class W>
inline void set_widget(octave_classdef* cls, const W* wdg) {
	cls->set_property(0, XWIDGETS_POINTER_PROPERTY, reinterpret_cast<intptr_t>(wdg));
}

template <class V, class T>
inline void set_property(xp::xproperty<V, T>& property, const octave_value& value) {
	V vec = ov<V>::from(value);
	property = vec;
}

template <class W, auto P>
inline octave_value_list set_property(const octave_value_list& args, int) {
	set_property(get_widget<W>(args(0).classdef_object_value())->*P, args(1));
	return ovl();
}

template <class V, class T>
inline octave_value get_property(xp::xproperty<V, T>& property) {
	return ov<V>::to(property);
}

template <class W, auto P>
inline octave_value_list get_property(const octave_value_list& args, int) {
	return get_property(get_widget<W>(args(0).classdef_object_value())->*P);
}

template <class W, auto P>
inline octave_value_list set_callback(octave::interpreter& interpreter, const octave_value_list& args, int) {
	(get_widget<W>(args(0).classdef_object_value())->*P)(
		[callback = args(1), &interpreter]() {
			interpreter.feval(callback);
		});

	return ovl();
}

template <class W>
inline octave_value_list constructor(const octave_value_list& args, int) {
	set_widget(args(0).classdef_object_value(), new W);
	return args;
}

template <class W, auto P>
inline octave_value_list observe(octave::interpreter& interpreter, const octave_value_list& args, int) {
	W* w = get_widget<W>(args(0).classdef_object_value());

	w->observe((w->*P).name(), [callback = args(1), &interpreter](const auto&) {
		interpreter.feval(callback);
	});

	return ovl();
};

template <class W>
inline octave_value_list destructor(const octave_value_list& args, int) {
	delete get_widget<W>(args(0).classdef_object_value());
	return ovl();
};

template <class W, auto P>
inline void xwidgets_add_callback(octave::interpreter& interpreter, octave::cdef_class& cls, std::string name) {
	octave::cdef_manager& cm = interpreter.get_cdef_manager();

	// Add basic methods
	cls.install_method(cm.make_method(cls, name, set_callback<W, P>));
}

inline void xwidgets_add_method(octave::interpreter& interpreter, octave::cdef_class& cls, std::string name, octave_builtin::fcn ff) {
	octave::cdef_manager& cm = interpreter.get_cdef_manager();

	cls.install_method(cm.make_method(cls, name, ff));
}

inline void xwidgets_add_method(octave::interpreter& interpreter, octave::cdef_class& cls, std::string name, octave_builtin::meth mm) {
	octave::cdef_manager& cm = interpreter.get_cdef_manager();

	cls.install_method(cm.make_method(cls, name, mm));
}

template <class W, auto P>
inline void xwidgets_add_property(octave::interpreter& interpreter, octave::cdef_class& cls, std::string name, bool ro = false) {
	octave::cdef_manager& cm = interpreter.get_cdef_manager();

	std::string kname = cls.get_name();

	using namespace xeus_octave::utils;

	if (ro) {
		xwidgets_add_method(interpreter, cls, name, get_property<W, P>);
	} else {
		cls.install_property(cm.make_property(
			cls, name,
			make_fcn_handle(get_property<W, P>, kname + ">get." + name), "public",
			make_fcn_handle(set_property<W, P>, kname + ">set." + name), "public"));

		cls.install_method(cm.make_method(cls, "observe_" + name, observe<W, P>));
	}
}

template <class W>
inline octave::cdef_class xwidgets_make_class(octave::interpreter& interpreter, std::string name) {
	octave::cdef_manager& cm = interpreter.get_cdef_manager();

	// Build the class type
	octave::cdef_class cls = cm.make_class(name, cm.find_class(XWIDGETS_BASE_CLASS_NAME));

	// Add basic methods (constructor and destructor)
	xwidgets_add_method(interpreter, cls, name, constructor<W>);
	xwidgets_add_method(interpreter, cls, "delete", destructor<W>);

	// Add xobject properties
	xwidgets_add_property<W, &W::_model_module>(interpreter, cls, "_ModelModule", true);
	xwidgets_add_property<W, &W::_model_module_version>(interpreter, cls, "_ModelModuleVersion", true);
	xwidgets_add_property<W, &W::_model_name>(interpreter, cls, "_ModelName", true);
	xwidgets_add_property<W, &W::_view_module>(interpreter, cls, "_ViewModule", true);
	xwidgets_add_property<W, &W::_view_module_version>(interpreter, cls, "_ViewModuleVersion", true);
	xwidgets_add_property<W, &W::_view_name>(interpreter, cls, "_ViewName", true);

	// Add xwidget properties
	xwidgets_add_property<W, &W::_dom_classes>(interpreter, cls, "_DomClasses");
	// TODO: add layout

	// Register the constructor
	interpreter.get_symbol_table().install_built_in_function(name, cls.get_constructor_function());

	return cls;
}
// namespace utils

void register_all(octave::interpreter& interpreter);
}  // namespace xeus_octave::widgets

#endif