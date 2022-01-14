/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_UTILS_HPP
#define XEUS_OCTAVE_UTILS_HPP

#include <octave/ov-builtin.h>
#include <octave/ov-fcn-handle.h>
#include <octave/ov.h>

#include <iostream>

#include "xeus/xguid.hpp"
#include "xtl/xoptional.hpp"

namespace xeus_octave::utils {

/**
 * octave_value <==> standard/xeus types conversion
 */
template <class T>
struct ov;

// Default conversion
template <class T>
struct ov {
	static inline T from(const octave_value &v) {
		return octave_value_extract<T>(v);
	}
	static inline octave_value to(const T &v) {
		return octave_value(v);
	}
};

// String conversion
template <>
struct ov<std::string> {
	static inline std::string from(const octave_value &v) {
		return v.string_value();
	}
	static inline octave_value to(const std::string &v) {
		return octave_value(v);
	}
};

// xguid
template <>
struct ov<xeus::xguid> {
	static inline xeus::xguid from(const octave_value &v) {
		return xeus::xguid(ov<std::string>::from(v));
	}
	static inline octave_value to(const xeus::xguid &v) {
		return ov<std::string>::to(v);
	}
};

// Forward vectors
template <class T>
struct ov<std::vector<T>> {
	static inline std::vector<T> from(const octave_value &v) {
		auto cell = v.cell_value();
		std::vector<T> r;

		for (Cell::size_type i = 0; i < cell.size(0); i++)
			r.push_back(ov<T>::from(cell(i)));

		return r;
	}

	static inline octave_value to(const std::vector<T> &v) {
		Cell a;
		octave_idx_type i = 0;

		a.resize(dim_vector(v.size(), 1));

		for (auto e : v)
			a.elem(i++, 0) = ov<T>::to(e);

		return a;
	}
};

// Forward the xoptional template
template <class T>
struct ov<xtl::xoptional<T>> {
	static inline xtl::xoptional<T> from(const octave_value &v) {
		if (v.is_defined())
			return xtl::xoptional<T>(ov<T>::from(v));

		return xtl::xoptional<T>();
	}

	static inline octave_value to(const xtl::xoptional<T> &v) {
		if (v.has_value())
			return ov<T>::to(v.value());

		return octave_value();
	}
};

inline void x() {
	xtl::xoptional<std::string> m;

	xeus_octave::utils::ov<xtl::xoptional<std::string>>::to(m);
}

inline octave_value make_fcn_handle(octave_builtin::fcn ff, const std::string &nm) {
	octave_value fcn(new octave_builtin(ff, nm));
	return octave_value(new octave_fcn_handle(fcn));
}

}  // namespace xeus_octave::utils

#endif