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

#ifndef PLOTLY_TOOLKIT_H
#define PLOTLY_TOOLKIT_H

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <octave/graphics-handle.h>
#include <octave/graphics-toolkit.h>
#include <octave/graphics.h>
#include <octave/interpreter.h>
#include <octave/ovl.h>
#include <octave/parse.h>
#include <octave/str-vec.h>
#include <nlohmann/json.hpp>

#include "xeus-octave/xinterpreter.hpp"

using namespace nlohmann;
using namespace octave;

namespace xeus_octave {

class plotly_graphics_toolkit : public octave::base_graphics_toolkit {
public:
	plotly_graphics_toolkit(octave::interpreter& interp)
		: base_graphics_toolkit("plotly"),
		  m_interpreter(interp) {}

	bool is_valid() const override { return true; }

	bool initialize(const octave::graphics_object& go) override;
	void redraw_figure(const octave::graphics_object& go) const override;
	void show_figure(const octave::graphics_object& go) const override;

private:
	/**
	 * Get the string suffix to append to plotly objects (eg xaxis, yaxis, scene
	 * polar), when more than one is present. The suffix for the first one is
	 * always "" (empty), then 1,2,3...
	 */
	inline std::string getObjectNumber(const octave::graphics_object& o, std::map<std::string, std::vector<unsigned long>>& ids) const {
		double h = o.get_handle().value();
		unsigned long id = *(unsigned long*) &h;
		std::string type;

		if (o.type() == "axes") {
			const axes::properties& axisProperties = dynamic_cast<const axes::properties&>(o.get_properties());

			if (axisProperties.get_tag() == "polaraxes")
				type = "polar";
			else if (!axisProperties.get_is2D())
				type = "scene";
			else
				type = "axis";
		}

		auto& idlist = ids[type];

		if (idlist.size() == 0) {
			idlist.push_back(id);
			return "";
		} else {
			auto it = std::find(idlist.begin(), idlist.end(), id);

			if (it == idlist.end()) {
				idlist.push_back(id);
				return std::to_string(idlist.size());
			} else {
				int d = std::distance(idlist.begin(), it);

				if (d == 0)
					return "";
				else
					return std::to_string(d + 1);
			}
		}
	}

	/**
	 * Get a vector of all the children of the @go octave::graphics_object
	 */
	std::vector<octave::graphics_object> children(const octave::graphics_object& go, bool all = false) const;

	/**
	 * Convert an octave color matrix to a css rgb string
	 */
	inline std::string matrix2rgb(const Matrix color) const {
		return "rgb(" + std::to_string((int) (color(0) * 255)) + "," + std::to_string((int) (color(1) * 255)) + "," + std::to_string((int) (color(2) * 255)) + ")";
	}

	inline std::string matrix2rgba(const Matrix color, const double alpha) const {
		return "rgba(" + std::to_string((int) (color(0) * 255)) + "," + std::to_string((int) (color(1) * 255)) + "," + std::to_string((int) (color(2) * 255)) + "," + std::to_string(alpha) + ")";
	}

	/**
	 * Convert an octave column matrix to a std::vector
	 */
	inline std::vector<double> matrixcol2vec(const Matrix m) const {
		std::vector<double> d;

		for (int i = 0; i < m.cols(); i++) {
			d.push_back(m(i));
		}

		return d;
	}

	/**
	 * Convert a string according to its format
	 */
	std::string convertText(std::string text, std::string format = "none") const;

	/**
	 * Fill the text properties
	 */
	void text(json& obj,
			  std::string text,
			  std::string interpreter,
			  Matrix color,
			  double fontSize) const;

	/**
	 * Fill the axis properties
	 */
	void axis(json& axis,
			  bool visible,
			  std::string scale,
			  std::string location,
			  Matrix range,
			  bool reverse,
			  bool box,
			  double lineWidth,
			  double fontSize,
			  Matrix _color,
			  Matrix _ticks,
			  string_vector _tickLabels,
			  Matrix _minorTicks,
			  bool showMinorTicks,
			  std::string tickDir,
			  double tickRotation,
			  std::string tickInterpreter,
			  bool showGrid,
			  Matrix _gridColor = Matrix(),
			  double gridAlpha = 0) const;

	/**
	 * Fill the polar axis properties
	 */
	void polarAxis(json& axis,
				   Matrix _ticks,
				   double fontSize) const;

	/**
	 * Fill the legend properties
	 */
	void legend(json& legend,
				Matrix position,
				bool box,
				double lineWidth,
				Matrix backgroundColor) const;

	/**
	 * Fill the line properties
	 */
	void line(json& line,
			  bool visible,
			  std::string type,
			  Matrix xdata,
			  Matrix ydata,
			  Matrix zdata,
			  std::string marker,
			  std::string lineStyle,
			  Matrix lineColor,
			  double lineWidth,
			  double markerSize) const;

	/**
	 * Fill the (3d) surface properties
	 */
	void surface(json& surf,
				 bool visible,
				 Matrix xdata,
				 Matrix ydata,
				 Matrix zdata,
				 Matrix cdata,
				 Matrix colorMap,
				 Matrix clim) const;

	/**
	 * Add a legend entry if needed
	 */
	void setLegendVisibility(json& data,
							 std::string name) const;

private:
	octave::interpreter& m_interpreter;
};

}  // namespace xeus_octave

#endif
