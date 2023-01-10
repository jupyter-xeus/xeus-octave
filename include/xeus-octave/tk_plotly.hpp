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

#ifndef XEUS_OCTAVE_PLOTLY_TOOLKIT_H
#define XEUS_OCTAVE_PLOTLY_TOOLKIT_H

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <octave/graphics-handle.h>
#include <octave/graphics-toolkit.h>
#include <octave/graphics.h>
#include <octave/interpreter.h>
#include <octave/ovl.h>
#include <octave/parse.h>
#include <octave/str-vec.h>

#include "xeus-octave/xinterpreter.hpp"

namespace nl = nlohmann;

namespace xeus_octave::tk::plotly
{

class plotly_graphics_toolkit : public octave::base_graphics_toolkit
{
public:

  plotly_graphics_toolkit(octave::interpreter& interp) : base_graphics_toolkit("plotly"), m_interpreter(interp) {}

  bool is_valid() const override { return true; }

  bool initialize(octave::graphics_object const& go) override;
  void redraw_figure(octave::graphics_object const& go) const override;
  void show_figure(octave::graphics_object const& go) const override;

private:

  /**
   * Get the string suffix to append to plotly objects (eg xaxis, yaxis, scene
   * polar), when more than one is present. The suffix for the first one is
   * always "" (empty), then 1,2,3...
   */
  std::string
  getObjectNumber(octave::graphics_object const& o, std::map<std::string, std::vector<unsigned long>>& ids) const;

  /**
   * Get a vector of all the children of the graphics object @p go.
   */
  std::vector<octave::graphics_object> children(octave::graphics_object const& go, bool all = false) const;

  /**
   * Fill the text properties
   */
  void text(nl::json& obj, std::string text, std::string interpreter, Matrix color, double fontSize) const;

  /**
   * Fill the axis properties
   */
  void axis(
    nl::json& axis,
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
    double gridAlpha = 0
  ) const;

  /**
   * Fill the polar axis properties
   */
  void polarAxis(nl::json& axis, Matrix _ticks, double fontSize) const;

  /**
   * Fill the legend properties
   */
  void legend(nl::json& legend, Matrix position, bool box, double lineWidth, Matrix backgroundColor) const;

  /**
   * Fill the line properties
   */
  void line(
    nl::json& line,
    bool visible,
    std::string type,
    Matrix xdata,
    Matrix ydata,
    Matrix zdata,
    std::string marker,
    std::string lineStyle,
    Matrix lineColor,
    double lineWidth,
    double markerSize
  ) const;

  /**
   * Fill the (3d) surface properties
   */
  void surface(
    nl::json& surf, bool visible, Matrix xdata, Matrix ydata, Matrix zdata, Matrix cdata, Matrix colorMap, Matrix clim
  ) const;

  /**
   * Add a legend entry if needed
   */
  void setLegendVisibility(nl::json& data, std::string name) const;

private:

  octave::interpreter& m_interpreter;
};

void register_all(octave::interpreter& interpreter);

}  // namespace xeus_octave::tk::plotly

#endif  // XEUS_OCTAVE_PLOTLY_TOOLKIT_H
