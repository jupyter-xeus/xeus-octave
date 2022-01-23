/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_UTILS_HPP
#define XEUS_OCTAVE_UTILS_HPP

#include <octave/interpreter.h>
#include <octave/ov-builtin.h>
#include <octave/ov-fcn-handle.h>
#include <octave/ov-null-mat.h>
#include <octave/ov.h>

#include <iostream>

#include "xeus/xguid.hpp"
#include "xtl/xoptional.hpp"

namespace xeus_octave::utils {

template <class T>
inline void from_ov(const octave_value &v, T &n) {
	n = octave_value_extract<T>(v);
}

inline void from_ov(const octave_value &v, std::string &n) {
	n = v.string_value();
}

inline void from_ov(const octave_value &v, int &n) {
	n = v.int_value();
}

inline void from_ov(const octave_value &v, xeus::xguid &n) {
	std::string s;
	from_ov(v, s);
	n = xeus::xguid(s);
}

template <class T>
inline void from_ov(const octave_value &v, std::vector<T> &n) {
	Cell cell = v.cell_value();

	for (int i = 0; i < cell.numel(); i++) {
		T e;
		from_ov(cell(0, i), e);
		n.push_back(e);
	}
}

inline void from_ov(const octave_value &v, std::vector<char> &n) {
	if (v.is_string()) {
		std::string s = v.string_value();
		std::copy(s.begin(), s.end(), std::back_inserter(n));
	} else {
		from_ov<char>(v, n);
	}
}

template <class T>
inline void from_ov(const octave_value &v, xtl::xoptional<T> &n) {
	if (!v.isnull()) {
		T e;
		from_ov(v, e);
		n = e;
	} else
		n = xtl::xoptional<T>();
}

template <class T>
inline void to_ov(octave_value &n, const T &v) {
	n = octave_value(v);
}

template <class T>
inline void to_ov(octave_value &n, const std::vector<T> &v) {
	Cell a;
	octave_idx_type i = 0;

	a.resize(dim_vector(v.size(), 1));

	for (auto e : v)
		to_ov(a.elem(i++, 0), e);

	n = a;
}

template <class T>
inline void to_ov(octave_value &n, const xtl::xoptional<T> &v) {
	if (v.has_value())
		to_ov(n, v.value());
	else
		n = octave_null_matrix::instance;
}

inline void to_ov(octave_value &n, const xeus::xguid &v) {
	to_ov(n, std::string(v));
}

inline octave_value make_fcn_handle(octave_builtin::fcn ff, const std::string &nm) {
	octave_value fcn(new octave_builtin(ff, nm));
	return octave_value(new octave_fcn_handle(fcn));
}

inline void add_native_binding(octave::interpreter &interpreter, const std::string &name, octave_builtin::fcn ff) {
	octave_builtin *fcn = new octave_builtin(ff, name, __FILE__, "");
	interpreter.get_symbol_table().install_built_in_function(name, fcn);
}

}  // namespace xeus_octave::utils

#endif