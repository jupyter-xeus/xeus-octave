/*
 * Copyright (C) 2020 Giulio Girardi.
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

#include "display.hpp"

#include <octave/cdef-package.h>
#include <octave/defun-int.h>
#include <octave/interpreter.h>
#include <octave/symtab.h>

#include <nlohmann/json.hpp>

#include "xoctave_interpreter.hpp"
#include "xeus/xinterpreter.hpp"

namespace xoctave::display {

namespace {

octave_value_list display_data(const octave_value_list& args, int /*nargout*/) {
	if (args.length() < 2 || args.length() > 4)
		print_usage();

	std::string type = args(0).xstring_value("TYPE must be a string");
	std::string value = args(1).xstring_value("VALUE must be a string");

	nlohmann::json d;
	nlohmann::json metadata;

	if (args.length() > 2 && args(2).xbool_value("ENCODE must be a boolean flag"))
		d[type] = nlohmann::json::parse(value);
	else
		d[type] = value;

	if (args.length() > 3)
		metadata[type] = nlohmann::json::parse(args(3).xstring_value("METADATA must be a json string"));

	dynamic_cast<xoctave::xoctave_interpreter&>(xeus::get_interpreter()).do_display_data(d, metadata);

	return ovl();
}

octave_value_list override_path(const octave_value_list& args, int /*nargout*/) {
	if (args.length() != 0)
		print_usage();

	return ovl(XOCTAVE_OVERRIDE_PATH);
}

/**
 * This function is much faster than its equivalent implementation in octave
 * a 5000 elements matrix takes 9 ms to render with this function, while 4 s (!)
 * with the native octave function
 */
octave_value_list matrix_to_html(const octave_value_list& args, int /*nargout*/) {
	std::ostringstream t;

	if (args.length() != 3)
		print_usage();

	std::string name = args(0).xstring_value("VALUE must be a string");
	std::string type = args(2).xstring_value("TYPE must be a string");

	Matrix m = args(1).matrix_value();
	ComplexMatrix cx = args(1).complex_matrix_value();

	t << "<table>"
	  << "<tr>";
	t << "<th><i>" << name << "<i></th>";

	for (int c = 0; c < m.cols(); c++) {
		t << "<th>" << c + 1 << "</th>";
	}

	t << "</tr>";

	for (int r = 0; r < m.rows(); r++) {
		t << "<tr>";
		t << "<th>" << r + 1 << "</th>";

		for (int c = 0; c < m.cols(); c++) {
			t << "<td>";
			if (type == "bool matrix")
				t << (m(r, c) > 0 ? "<span style='color: green;'>T</span>" : "<span style='color: red;'>F</span>");
			else if (type == "complex matrix")
				t << cx(r, c).real() << "<br>" << cx(r, c).imag() << "<i>i</i>";
			else
				t << m(r, c);
			t << "</td>";
		}

		t << "</tr>";
	}

	t << "</table>";

	return ovl(t.str());
}

octave_value_list matrix_to_latex(const octave_value_list& args, int /*nargout*/) {
	std::ostringstream l;

	if (args.length() != 3)
		print_usage();

	std::string name = args(0).xstring_value("VALUE must be a string");
	std::string type = args(2).xstring_value("TYPE must be a string");

	Matrix m = args(1).matrix_value();

	l << "$$ " << name << " = ";

	if (type == "range") {
		l << m(0) << "," << m(1) << ",\\dots," << m(m.cols() - 1);
	} else {
		l << "\\begin{bmatrix}";

		for (int r = 0; r < m.rows(); r++) {
			for (int c = 0; c < m.cols(); c++) {
				l << " " << m(r, c) << " ";

				if (c < m.cols() - 1)
					l << "&";
			}

			if (r < m.rows() - 1)
				l << "\\\\";
		}

		l << "\\end{bmatrix}";
	}

	l << "$$";

	return ovl(l.str());
}

}  // namespace

void register_all(octave::cdef_package& p, octave::interpreter& i) {
	auto &s = i.get_symbol_table();

	auto display_data_func = new octave_builtin(display_data, "display_data", __FILE__, "");
	auto override_path_func = new octave_builtin(override_path, "OVERRIDE_PATH", __FILE__, "");
	auto matrix_to_html_func = new octave_builtin(matrix_to_html, "matrix_to_html", __FILE__, "");
	auto matrix_to_latex_func = new octave_builtin(matrix_to_latex, "matrix_to_latex", __FILE__, "");

	p.install_function(display_data_func, "display_data");
	p.install_function(override_path_func, "OVERRIDE_PATH");
	p.install_function(matrix_to_html_func, "matrix_to_html");
	p.install_function(matrix_to_latex_func, "matrix_to_latex");

	s.install_built_in_function("xoctave.display_data", display_data_func);
	s.install_built_in_function("xoctave.OVERRIDE_PATH", override_path_func);
	s.install_built_in_function("xoctave.matrix_to_html", matrix_to_html_func);
	s.install_built_in_function("xoctave.matrix_to_latex", matrix_to_latex_func);
}

}  // namespace xoctave::display