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
inline void set_property(xp::xproperty<V, T>& property, octave_value value) {
	if constexpr (std::is_same_v<V, double>)
		property = value.double_value();
	else if constexpr (std::is_same_v<V, std::string>)
		property = value.string_value();
	else
		std::cerr << "Type " << typeid(V).name() << " is not handled" << std::endl;
}

template <class W, auto P>
inline octave_value_list set_property(const octave_value_list& args, int) {
	set_property(get_widget<W>(args(0).classdef_object_value())->*P, args(1));
	return ovl();
}

template <class W, auto P>
inline octave_value_list get_property(const octave_value_list& args, int) {
	return ovl((get_widget<W>(args(0).classdef_object_value())->*P)());
}

template <class W>
inline octave_value_list constructor(const octave_value_list& args, int) {
	set_widget(args(0).classdef_object_value(), new W);
	return args;
}

template <class W>
inline octave_value_list display(const octave_value_list& args, int) {
	get_widget<W>(args(0).classdef_object_value())->display();
	return ovl();
}

template <class W>
inline octave_value_list destructor(const octave_value_list& args, int) {
	delete get_widget<W>(args(0).classdef_object_value());
	return ovl();
};

template <class W>
inline octave::cdef_class xwidgets_make_class(octave::interpreter& interpreter, std::string name) {
	octave::cdef_manager& cm = interpreter.get_cdef_manager();

	// Build the class type
	octave::cdef_class klass = cm.make_class(name);

	// Add basic methods
	klass.install_method(cm.make_method(klass, name, constructor<W>));
	klass.install_method(cm.make_method(klass, "display", display<W>));
	klass.install_method(cm.make_method(klass, "delete", destructor<W>));

	// Register the constructor
	interpreter.get_symbol_table().install_built_in_function(name, klass.get_constructor_function());

	return klass;
}

template <class W, auto P>
inline void xwidgets_add_property(octave::interpreter& interpreter, octave::cdef_class& klass, std::string name) {
	octave::cdef_manager& cm = interpreter.get_cdef_manager();

	std::string kname = klass.get_name();

	octave_fcn_handle* getter = new octave_fcn_handle(new octave_builtin(get_property<W, P>, kname + ">get." + name));
	octave_fcn_handle* setter = new octave_fcn_handle(new octave_builtin(set_property<W, P>, kname + ">set." + name));

	klass.install_property(cm.make_property(klass, name, getter, "public", setter, "public"));
}

void register_slider(octave::interpreter& interpreter) {
	using W = xw::slider<double>;

	octave::cdef_class slider = xwidgets_make_class<W>(interpreter, "slider");

	xwidgets_add_property<W, &W::value>(interpreter, slider, "value");
	xwidgets_add_property<W, &W::readout_format>(interpreter, slider, "readout_format");
}

}  // namespace

namespace xeus_octave::widgets {

void register_all(octave::interpreter& interpreter) {
	register_slider(interpreter);
}

}  // namespace xeus_octave::widgets