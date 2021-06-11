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

#include <octave/defun-int.h>
#include <octave/interpreter.h>
#include <octave/oct-map.h>
#include <octave/symtab.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>

#include "xeus/xinterpreter.hpp"
#include "xoctave_interpreter.hpp"

namespace xoctave::display {

namespace {

octave_value_list display_data(const octave_value_list& args, int /*nargout*/) {
	if (args.length() < 1 || args.length() > 2)
		print_usage();

	nlohmann::json data;
	octave_map d = args(0).xmap_value("DATA must be a map");

	for (auto value : d) {
		auto v = d.contents(value.second);
		if (value.first == "application/json")
			data[value.first] = nlohmann::json::parse(v(0).xstring_value("DATA contents must be strings"));
		else
			data[value.first] = v(0).xstring_value("DATA contents must be strings");
	}

	nlohmann::json metadata;

	if (args.length() > 1) {
		octave_map m = args(0).xmap_value("METADATA must be a map");

		for (auto value : m) {
			auto v = m.contents(value.second);
			data[value.first] = v(0).xstring_value("METADATA contents must be strings");
		}
	}

	dynamic_cast<xoctave::xoctave_interpreter&>(xeus::get_interpreter()).display_data(data, metadata);

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

std::string latex_fix_sci_not(std::string text) {
	text = std::regex_replace(text, std::regex("e\\+[0]*([0-9]+)"), "\\mathrm{ᴇ}{$1}");
	text = std::regex_replace(text, std::regex("e\\-[0]*([0-9]+)"), "\\mathrm{ᴇ\\text{-}}{$1}");

	return text;
}

octave_value_list latex_fix_sci_not(const octave_value_list& args, int /*nargout*/) {
	if (args.length() != 1)
		print_usage();

	std::string text = args(0).xstring_value("IN must be a string");

	return ovl(latex_fix_sci_not(text));
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

	return ovl(latex_fix_sci_not(l.str()));
}

}  // namespace

void register_all(octave::interpreter& i) {
	auto& s = i.get_symbol_table();

	auto display_data_func = new octave_builtin(display_data, "display_data", __FILE__, "");
	auto override_path_func = new octave_builtin(override_path, "XOCTAVE_OVERRIDE_PATH", __FILE__, "");
	auto matrix_to_html_func = new octave_builtin(matrix_to_html, "__matrix_to_html__", __FILE__, "");
	auto matrix_to_latex_func = new octave_builtin(matrix_to_latex, "__matrix_to_latex__", __FILE__, "");
	auto latex_fix_sci_not_func = new octave_builtin(latex_fix_sci_not, "__latex_fix_sci_not__", __FILE__, "");

	s.install_built_in_function("display_data", display_data_func);
	s.install_built_in_function("XOCTAVE_OVERRIDE_PATH", override_path_func);
	s.install_built_in_function("__matrix_to_html__", matrix_to_html_func);
	s.install_built_in_function("__matrix_to_latex__", matrix_to_latex_func);
	s.install_built_in_function("__latex_fix_sci_not__", latex_fix_sci_not_func);
}

}  // namespace xoctave::display