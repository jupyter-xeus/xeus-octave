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

#include <algorithm>
#include <cmath>
#include <complex>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include <nlohmann/json.hpp>
#include <octave/graphics-handle.h>
#include <octave/graphics-toolkit.h>
#include <octave/graphics.h>
#include <octave/ov.h>
#include <octave/ovl.h>
#include <octave/parse.h>
#include <octave/quit.h>
#include <octave/str-vec.h>
#include <octave/text-engine.h>
#include <octave/utils.h>
#include <octave/version.h>

#include "xeus-octave/plotstream.hpp"
#include "xeus-octave/tex2html.hpp"
#include "xeus-octave/tk_plotly.hpp"

namespace oc = octave;
namespace nl = nlohmann;

namespace xeus_octave::tk::plotly
{

bool plotly_graphics_toolkit::initialize(oc::graphics_object const& go)
{
  if (go.isa("figure"))
  {
    // Create a new object id and store it in the figure
    setPlotStream(go, xeus::new_xguid());
    show_figure(go);

    return true;
  }

  return false;
}

void plotly_graphics_toolkit::redraw_figure(oc::graphics_object const& go) const
{
  // Retrieve the figure id
  std::string id = getPlotStream<std::string>(go);

  if (go.isa("figure"))
  {
    std::map<std::string, std::vector<unsigned long>> ids;
    auto& figureProperties = dynamic_cast<oc::figure::properties&>(oc::graphics_object(go).get_properties());
    Matrix figurePosition = figureProperties.get_position().matrix_value();
    nl::json plot, output;

    // Setting margins to 0 because octave positions its axes considering
    // the margins
    plot["layout"]["margin"]["l"] = 0;
    plot["layout"]["margin"]["r"] = 0;
    plot["layout"]["margin"]["b"] = 0;
    plot["layout"]["margin"]["t"] = 0;

    // Setting width and height properties
    plot["layout"]["width"] = figurePosition(2);
    plot["layout"]["height"] = figurePosition(3);

    // Tooltip on the closest point
    plot["layout"]["hovermode"] = "closest";

    // We draw manually the legend
    plot["layout"]["showlegend"] = false;

    // Background color
    plot["layout"]["plot_bgcolor"] = "rgba(0,0,0,0)";

    // Figures contain axes and hggroups (not implemented for now) as children
    for (auto ax : children(go))
      if (ax.isa("axes"))
      {
        auto& axisProperties = dynamic_cast<oc::axes::properties&>(ax.get_properties());
#if OCTAVE_MAJOR_VERSION >= 6
        auto xlabel = m_interpreter.get_gh_manager().get_object(axisProperties.get_xlabel());
        auto ylabel = m_interpreter.get_gh_manager().get_object(axisProperties.get_ylabel());
        auto zlabel = m_interpreter.get_gh_manager().get_object(axisProperties.get_zlabel());
#else
        auto xlabel = gh_manager::get_object(axisProperties.get_xlabel());
        auto ylabel = gh_manager::get_object(axisProperties.get_ylabel());
        auto zlabel = gh_manager::get_object(axisProperties.get_zlabel());
#endif
        auto& xlabelProperties = dynamic_cast<oc::text::properties&>(xlabel.get_properties());
        auto& ylabelProperties = dynamic_cast<oc::text::properties&>(ylabel.get_properties());
        auto& zlabelProperties = dynamic_cast<oc::text::properties&>(zlabel.get_properties());

        Matrix axisPosition = axisProperties.get_position().matrix_value();
        std::string axNumber = getObjectNumber(ax, ids);

        bool isLegend = !ax.get("tag").isempty() && ax.get("tag").string_value() == "legend";

        if (!ax.get("tag").isempty() && ax.get("tag").string_value() == "polaraxes")
        {
          std::string p = "polar" + axNumber;

          // Setting domain, which is the position of the axis of the figure
          // (percentage)
          plot["layout"][p]["domain"]["x"] = {axisPosition(0), axisPosition(0) + axisPosition(2)};
          plot["layout"][p]["domain"]["y"] = {axisPosition(1), axisPosition(1) + axisPosition(3)};

          polarAxis(
            plot["layout"][p]["radialaxis"], axisProperties.get("rtick").matrix_value(), axisProperties.get_fontsize()
          );

          polarAxis(
            plot["layout"][p]["angularaxis"], axisProperties.get("ttick").matrix_value(), axisProperties.get_fontsize()
          );
        }
        else if (axisProperties.get_is2D())
        {
          std::string x = "xaxis" + axNumber;
          std::string y = "yaxis" + axNumber;

          // Setting domain, which is the position of the axis of the figure
          // (percentage)
          plot["layout"][x]["domain"] = {axisPosition(0), axisPosition(0) + axisPosition(2)};
          plot["layout"][y]["domain"] = {axisPosition(1), axisPosition(1) + axisPosition(3)};

          axis(
            plot["layout"][x],
            axisProperties.is_visible(),
            axisProperties.get_xscale(),
            axisProperties.get_xaxislocation(),
            axisProperties.get_xlim().matrix_value(),
            axisProperties.xdir_is("reverse"),
            axisProperties.is_box(),
            axisProperties.get_linewidth(),
            axisProperties.get_fontsize(),
            axisProperties.get_xcolor_rgb(),
            axisProperties.get_xtick().matrix_value(),
            axisProperties.get_xticklabel().string_vector_value(),
            axisProperties.get_xminortickvalues().matrix_value(),
            axisProperties.get_xminortick() == "on",
            axisProperties.get_tickdir(),
            axisProperties.get_xticklabelrotation(),
            axisProperties.get_ticklabelinterpreter(),
            axisProperties.is_xgrid() || axisProperties.is_xminorgrid(),
            axisProperties.get_gridcolor_rgb(),
            axisProperties.get_gridalpha()
          );

          axis(
            plot["layout"][y],
            axisProperties.is_visible(),
            axisProperties.get_yscale(),
            axisProperties.get_yaxislocation(),
            axisProperties.get_ylim().matrix_value(),
            axisProperties.ydir_is("reverse"),
            axisProperties.is_box(),
            axisProperties.get_linewidth(),
            axisProperties.get_fontsize(),
            axisProperties.get_ycolor_rgb(),
            axisProperties.get_ytick().matrix_value(),
            axisProperties.get_yticklabel().string_vector_value(),
            axisProperties.get_yminortickvalues().matrix_value(),
            axisProperties.get_yminortick() == "on",
            axisProperties.get_tickdir(),
            axisProperties.get_yticklabelrotation(),
            axisProperties.get_ticklabelinterpreter(),
            axisProperties.is_ygrid() || axisProperties.is_yminorgrid(),
            axisProperties.get_gridcolor_rgb(),
            axisProperties.get_gridalpha()
          );

          if (xlabel && xlabel.isa("text"))
            text(
              plot["layout"][x]["title"],
              xlabelProperties.get_string().string_value(),
              xlabelProperties.get_interpreter(),
              xlabelProperties.get_color_rgb(),
              xlabelProperties.get_fontsize()
            );

          if (ylabel && ylabel.isa("text"))
            text(
              plot["layout"][y]["title"],
              ylabelProperties.get_string().string_value(),
              ylabelProperties.get_interpreter(),
              ylabelProperties.get_color_rgb(),
              ylabelProperties.get_fontsize()
            );

          // Anchoring each axis to the other
          plot["layout"][x]["anchor"] = "y" + axNumber;
          plot["layout"][y]["anchor"] = "x" + axNumber;

          if (isLegend)
          {
            plot["layout"][x]["showspikes"] = false;
            plot["layout"][y]["showspikes"] = false;
            plot["layout"][x]["fixedrange"] = true;
            plot["layout"][y]["fixedrange"] = true;
          }
        }
        else
        {
          std::string s = "scene" + axNumber;

          // Setting domain, which is the position of the axis of the figure
          // (percentage)
          plot["layout"][s]["domain"]["x"] = {axisPosition(0), axisPosition(0) + axisPosition(2)};
          plot["layout"][s]["domain"]["y"] = {axisPosition(1), axisPosition(1) + axisPosition(3)};

          axis(
            plot["layout"][s]["xaxis"],
            axisProperties.is_visible(),
            axisProperties.get_xscale(),
            axisProperties.get_xaxislocation(),
            axisProperties.get_xlim().matrix_value(),
            !axisProperties.xdir_is("reverse"),
            axisProperties.is_box(),
            axisProperties.get_linewidth() + 1,
            axisProperties.get_fontsize(),
            axisProperties.get_xcolor_rgb(),
            axisProperties.get_xtick().matrix_value(),
            axisProperties.get_xticklabel().string_vector_value(),
            axisProperties.get_xminortickvalues().matrix_value(),
            axisProperties.get_xminortick() == "on",
            axisProperties.get_tickdir(),
            axisProperties.get_xticklabelrotation(),
            axisProperties.get_ticklabelinterpreter(),
            axisProperties.is_xgrid() || axisProperties.is_xminorgrid()
          );

          axis(
            plot["layout"][s]["yaxis"],
            axisProperties.is_visible(),
            axisProperties.get_yscale(),
            axisProperties.get_yaxislocation(),
            axisProperties.get_ylim().matrix_value(),
            !axisProperties.ydir_is("reverse"),
            axisProperties.is_box(),
            axisProperties.get_linewidth() + 1,
            axisProperties.get_fontsize(),
            axisProperties.get_ycolor_rgb(),
            axisProperties.get_ytick().matrix_value(),
            axisProperties.get_yticklabel().string_vector_value(),
            axisProperties.get_yminortickvalues().matrix_value(),
            axisProperties.get_yminortick() == "on",
            axisProperties.get_tickdir(),
            axisProperties.get_yticklabelrotation(),
            axisProperties.get_ticklabelinterpreter(),
            axisProperties.is_ygrid() || axisProperties.is_yminorgrid()
          );

          axis(
            plot["layout"][s]["zaxis"],
            axisProperties.is_visible(),
            axisProperties.get_zscale(),
            "none",
            axisProperties.get_zlim().matrix_value(),
            axisProperties.zdir_is("reverse"),
            axisProperties.is_box(),
            axisProperties.get_linewidth() + 1,
            axisProperties.get_fontsize(),
            axisProperties.get_zcolor_rgb(),
            axisProperties.get_ztick().matrix_value(),
            axisProperties.get_zticklabel().string_vector_value(),
            axisProperties.get_zminortickvalues().matrix_value(),
            axisProperties.get_zminortick() == "on",
            axisProperties.get_tickdir(),
            axisProperties.get_zticklabelrotation(),
            axisProperties.get_ticklabelinterpreter(),
            axisProperties.is_zgrid() || axisProperties.is_zminorgrid()
          );

          // Adding labels
          if (xlabel && xlabel.isa("text"))
            text(
              plot["layout"][s]["xaxis"]["title"],
              xlabelProperties.get_string().string_value(),
              xlabelProperties.get_interpreter(),
              xlabelProperties.get_color_rgb(),
              xlabelProperties.get_fontsize()
            );

          if (ylabel && ylabel.isa("text"))
            text(
              plot["layout"][s]["yaxis"]["title"],
              ylabelProperties.get_string().string_value(),
              ylabelProperties.get_interpreter(),
              ylabelProperties.get_color_rgb(),
              ylabelProperties.get_fontsize()
            );

          if (zlabel && zlabel.isa("text"))
            text(
              plot["layout"][s]["zaxis"]["title"],
              zlabelProperties.get_string().string_value(),
              zlabelProperties.get_interpreter(),
              zlabelProperties.get_color_rgb(),
              zlabelProperties.get_fontsize()
            );

          // Set projection type
          plot["layout"][s]["camera"]["projection"]["type"] = axisProperties.get_projection();
        }

        // Axes contain line, text, patch, surface, image, and light objects.
        for (auto d : children(ax))
        {
          unsigned long dNumber = plot["data"].size();

          if (d.isa("line"))
          {
            auto& lineProperties = dynamic_cast<oc::line::properties&>(d.get_properties());
            std::string type;

            // Set corresponding type and axes/scene
            if (!ax.get("tag").isempty() && ax.get("tag").string_value() == "polaraxes")
            {
              type = "scatterpolar";

              plot["data"][dNumber]["subplot"] = "polar" + axNumber;
            }
            else if (axisProperties.get_is2D())
            {
              type = "scatter";

              plot["data"][dNumber]["xaxis"] = "x" + axNumber;
              plot["data"][dNumber]["yaxis"] = "y" + axNumber;
            }
            else
            {
              type = "scatter3d";

              plot["data"][dNumber]["scene"] = "scene" + axNumber;
            }

            line(
              plot["data"][dNumber],
              lineProperties.is_visible(),
              type,
              lineProperties.get_xdata().matrix_value(),
              lineProperties.get_ydata().matrix_value(),
              lineProperties.get_zdata().matrix_value(),
              lineProperties.get_marker(),
              lineProperties.get_linestyle(),
              lineProperties.get_color_rgb(),
              lineProperties.get_linewidth(),
              lineProperties.get_markersize()
            );

            if (isLegend)
              plot["data"][dNumber]["hoverinfo"] = "none";

            setLegendVisibility(plot["data"][dNumber], lineProperties.get_displayname());
          }
          else if (d.isa("surface"))
          {
            auto& surfaceProperties = dynamic_cast<oc::surface::properties&>(d.get_properties());

            if (axisProperties.get_is2D())
            {
#ifndef NDEBUG
              std::clog << "2d surface not implemented" << std::endl;
#endif
            }
            else
            {
              plot["data"][dNumber]["scene"] = "scene" + axNumber;

              surface(
                plot["data"][dNumber],
                surfaceProperties.is_visible(),
                surfaceProperties.get_xdata().matrix_value(),
                surfaceProperties.get_ydata().matrix_value(),
                surfaceProperties.get_zdata().matrix_value(),
                surfaceProperties.get_cdata().matrix_value(),
                axisProperties.get("colormap").matrix_value(),
                surfaceProperties.get_clim().matrix_value()
              );

              setLegendVisibility(plot["data"][dNumber], surfaceProperties.get_displayname());
            }
          }
          else if (d.isa("text"))
          {
            auto& textProperties = dynamic_cast<oc::text::properties&>(d.get_properties());

            Matrix textPosition = textProperties.get_position().matrix_value();

            unsigned long aNumber = plot["layout"]["annotations"].size();

            plot["layout"]["annotations"][aNumber]["showarrow"] = false;

            plot["layout"]["annotations"][aNumber]["xref"] = "x" + axNumber;
            plot["layout"]["annotations"][aNumber]["yref"] = "y" + axNumber;

            plot["layout"]["annotations"][aNumber]["x"] = textPosition(0);
            plot["layout"]["annotations"][aNumber]["y"] = textPosition(1);

            plot["layout"]["annotations"][aNumber]["xanchor"] = textProperties.get_horizontalalignment();

            std::string valign = textProperties.get_verticalalignment();

            if (valign == "top" || valign == "cap")
              plot["layout"]["annotations"][aNumber]["yanchor"] = "top";
            else if (valign == "middle")
              plot["layout"]["annotations"][aNumber]["yanchor"] = "middle";
            else if (valign == "baseline" || valign == "bottom")
              plot["layout"]["annotations"][aNumber]["yanchor"] = "bottom";

            text(
              plot["layout"]["annotations"][aNumber],
              textProperties.get_string().string_value(),
              textProperties.get_interpreter(),
              textProperties.get_color_rgb(),
              textProperties.get_fontsize()
            );

#ifndef NDEBUG
            std::clog << textProperties.get_extent().matrix_value() << std::endl;
#endif
          }
          else if (d.isa("hggroup"))
          {
            auto components = children(d);
            auto& hggroupProperties = dynamic_cast<oc::hggroup::properties&>(d.get_properties());

            switch (components.size())
            {
            case 2:
              // We suppose that a line+line hggroup is a stem
              if (components[0].isa("line") && components[1].isa("line"))
              {
                auto& lineProperties = dynamic_cast<oc::line::properties&>(components[0].get_properties());
                std::string type;

                if (axisProperties.get_is2D())
                {
                  type = "scatter";

                  plot["data"][dNumber]["xaxis"] = "x" + axNumber;
                  plot["data"][dNumber]["yaxis"] = "y" + axNumber;
                }
                else
                {
                  type = "scatter3d";

                  plot["data"][dNumber]["scene"] = "scene" + axNumber;
                }

                line(
                  plot["data"][dNumber],
                  hggroupProperties.is_visible(),
                  type,
                  lineProperties.get_xdata().matrix_value(),
                  lineProperties.get_ydata().matrix_value(),
                  lineProperties.get_zdata().matrix_value(),
                  hggroupProperties.get("marker").string_value(),
                  hggroupProperties.get("linestyle").string_value(),
                  hggroupProperties.get("color").matrix_value(),
                  hggroupProperties.get("linewidth").double_value(),
                  hggroupProperties.get("markersize").double_value()
                );

                // Fix markers: by default markers would be
                // visible also on the bottom, so we make them
                // transparent
                std::string tempColor = plot["data"][dNumber]["line"]["color"];
                std::string tempMarkerColor = plot["data"][dNumber]["marker"]["color"];

                plot["data"][dNumber]["marker"]["line"]["color"] = {};
                plot["data"][dNumber]["marker"]["color"] = {};

                for (size_t i = 0; i < plot["data"][dNumber]["x"].size(); i += 3)
                {
                  plot["data"][dNumber]["marker"]["line"]["color"][i] = "rgba(0,0,0,0)";
                  plot["data"][dNumber]["marker"]["line"]["color"][i + 1] = tempColor;
                  plot["data"][dNumber]["marker"]["line"]["color"][i + 2] = "rgba(0,0,0,0)";

                  plot["data"][dNumber]["marker"]["color"][i] = "rgba(0,0,0,0)";
                  plot["data"][dNumber]["marker"]["color"][i + 1] = tempMarkerColor;
                  plot["data"][dNumber]["marker"]["color"][i + 2] = "rgba(0,0,0,0)";
                }
              }
              break;
            default:
              break;
            }

            setLegendVisibility(plot["data"][dNumber], hggroupProperties.get_displayname());
          }
        }
      }

    // Show the newly created plot
    nl::json data = nl::json::object();
    data["application/vnd.plotly.v1+json"] = std::move(plot);
    dynamic_cast<xoctave_interpreter&>(xeus::get_interpreter())
      .update_display_data(std::move(data), nl::json(nl::json::value_t::object), {{"display_id", id}});
  }
}

void plotly_graphics_toolkit::show_figure(oc::graphics_object const& go) const
{
  // Get an unique identifier for this object, to be used as a display id
  // in the display_data request for subsequent updates of the plot
  std::string id = getPlotStream<std::string>(go);

  // Display an empty figure (this is equivalent to the action of creating)
  // a window, and prepares a display with the correct display_id for
  // future updates
  dynamic_cast<xoctave_interpreter&>(xeus::get_interpreter())
    .display_data(nl::json(nl::json::value_t::object), nl::json(nl::json::value_t::object), {{"display_id", id}});
}

std::string plotly_graphics_toolkit::getObjectNumber(
  octave::graphics_object const& o, std::map<std::string, std::vector<unsigned long>>& ids
) const
{
  double h = o.get_handle().value();
  unsigned long id = *reinterpret_cast<unsigned long*>(&h);
  std::string type;

  if (o.type() == "axes")
  {
    auto const& axisProperties = dynamic_cast<octave::axes::properties const&>(o.get_properties());

    if (axisProperties.get_tag() == "polaraxes")
      type = "polar";
    else if (!axisProperties.get_is2D())
      type = "scene";
    else
      type = "axis";
  }

  auto& idlist = ids[type];

  if (idlist.size() == 0)
  {
    idlist.push_back(id);
    return "";
  }
  else
  {
    auto it = std::find(idlist.begin(), idlist.end(), id);

    if (it == idlist.end())
    {
      idlist.push_back(id);
      return std::to_string(idlist.size());
    }
    else
    {
      auto const dist = std::distance(idlist.begin(), it);
      if (dist == 0)
        return "";
      else
        return std::to_string(dist + 1);
    }
  }
}

std::vector<oc::graphics_object> plotly_graphics_toolkit::children(oc::graphics_object const& go, bool all) const
{
  Matrix c = all ? go.get_properties().get_all_children() : go.get_properties().get_children();
  auto len = c.numel();
  std::vector<oc::graphics_object> ret;

  for (auto i = len - 1; i >= 0; i--)
  {
    ret.push_back(m_interpreter.get_gh_manager().get_object(c(i)));
  }

  return ret;
}

namespace
{
/**
 * Convert an octave color matrix to a css rgb string
 */
std::string matrix2rgb(Matrix const& color)
{
  auto result = std::stringstream();
  result << "rgb(" << static_cast<int>(color(0) * 255) << "," << static_cast<int>(color(1) * 255) << ","
         << static_cast<int>(color(2) * 255) << ")";
  return std::move(result).str();
}

std::string matrix2rgba(Matrix const& color, double const alpha)
{
  auto result = std::stringstream();
  result << "rgba(" << static_cast<int>(color(0) * 255) << "," << static_cast<int>(color(1) * 255) << ","
         << static_cast<int>(color(2) * 255) << "," << alpha << ")";
  return std::move(result).str();
}

/**
 * Convert a string according to its format
 */
std::string convertText(std::string text, std::string format = "none")
{
  if (format == "latex")
  {
    return "$ " + text + " $";
  }
  else if (format == "tex")
  {
    oc::text_parser_tex tex = oc::text_parser_tex();
    tex_to_html html;
    tex.parse(text)->accept(html);
    return html;
  }
  else
    return text;
}

/**
 * Convert an octave column matrix to a std::vector
 */
std::vector<double> matrixcol2vec(Matrix const& m)
{
  assert(m.cols() >= 0);
  auto out = std::vector<double>(static_cast<std::size_t>(m.cols()));
  for (octave_idx_type i = 0; i < m.cols(); i++)
  {
    out[static_cast<std::size_t>(i)] = m(i);
  }
  return out;
}

}  // namespace

void plotly_graphics_toolkit::text(
  nl::json& obj, std::string text, std::string interpreter, Matrix color, double fontSize
) const
{
  obj["text"] = convertText(text, interpreter);

  // Set color and size
  obj["font"]["color"] = matrix2rgb(color);
  obj["font"]["size"] = fontSize;
}

void plotly_graphics_toolkit::axis(
  nl::json& axis,
  bool visible,
  std::string scale,
  std::string location,
  Matrix range,
  bool reverse,
  bool box,
  double _lineWidth,
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
  Matrix _gridColor,
  double gridAlpha
) const
{
  std::string color = matrix2rgb(_color);
  std::string gridColor = _gridColor.isempty() ? "" : matrix2rgba(_gridColor, gridAlpha);
  std::vector<double> ticks = matrixcol2vec(_ticks);
  std::vector<std::string> tickLabels;
  double lineWidth = std::max(1.0, _lineWidth);

  // Setting visibility
  axis["visible"] = visible;

  // Setting type
  axis["type"] = scale;

  // Setting range
  if (reverse)
    if (scale == "log")
      axis["range"] = {std::log10(range(1)), std::log10(range(0))};
    else
      axis["range"] = {range(1), range(0)};
  else if (scale == "log")
    axis["range"] = {std::log10(range(0)), std::log10(range(1))};
  else
    axis["range"] = {range(0), range(1)};

  // Setting axis side
  // TODO: When axis location is set to origin, the line passing
  // through the origin is shown, however the axis is still to
  // the side. This is due to a limitation in plotly.
  if (location == "origin")
  {
    axis["zeroline"] = true;
    axis["zerolinewidth"] = lineWidth;
    axis["zerolinecolor"] = color;
  }
  else
  {
    axis["side"] = location;
    axis["zeroline"] = false;
  }

  // Setting line
  axis["showline"] = true;
  axis["linecolor"] = color;
  axis["linewidth"] = lineWidth;

  // Configuring ticks
  axis["ticks"] = ticks.size() ? (tickDir == "in" ? "inside" : "outside") : "";

  if (showMinorTicks)
  {
    std::vector<double> minorTicks = matrixcol2vec(_minorTicks);
    std::vector<double> allTicks;

    allTicks.insert(allTicks.end(), ticks.begin(), ticks.end());
    allTicks.insert(allTicks.end(), minorTicks.begin(), minorTicks.end());

    std::sort(allTicks.begin(), allTicks.end());

    for (size_t i = 0; i < allTicks.size(); i++)
    {
      if (std::count(ticks.begin(), ticks.end(), allTicks[i]))
      {
        auto index = std::distance(ticks.begin(), std::find(ticks.begin(), ticks.end(), allTicks[i]));
        tickLabels.push_back(convertText(_tickLabels(index), tickInterpreter));
      }
      else
        tickLabels.push_back("");
    }

    axis["tickvals"] = allTicks;
  }
  else
  {
    for (int i = 0; i < _tickLabels.numel(); i++)
    {
      tickLabels.push_back(convertText(_tickLabels(i), tickInterpreter));
    }

    axis["tickvals"] = ticks;
  }

  // See https://github.com/plotly/plotly.js/issues/2885
  axis["ticktext"] = tickLabels;
  axis["tickangle"] = tickRotation;
  axis["tickcolor"] = color;
  axis["ticklen"] = 3;
  axis["tickfont"]["size"] = fontSize;
  axis["tickfont"]["color"] = color;

  // Configuring box
  if (box)
  {
    if (ticks.size())
      axis["mirror"] = "ticks";
    else
      axis["mirror"] = true;
  }
  else
  {
    axis["mirror"] = false;
    axis["mirror"] = false;
  }

  // Configuring grid
  axis["showgrid"] = showGrid;
  axis["gridcolor"] = gridColor;
  axis["gridwidth"] = lineWidth;

  // Disable autorange
  axis["autorange"] = false;

  // Disable exponent format
  axis["exponentformat"] = "none";

  // Configuring spike lines (cursor)
  axis["spikedash"] = "solid";
  axis["spikethickness"] = 1;
  axis["spikemode"] = "across";
  axis["spikesides"] = false;
}

void plotly_graphics_toolkit::polarAxis(nl::json& axis, Matrix _ticks, double fontSize) const
{
  std::vector<double> tick = matrixcol2vec(_ticks);

  axis["tickmode"] = "array";
  axis["tickvals"] = tick;
  axis["size"] = fontSize;
}

void plotly_graphics_toolkit::legend(
  nl::json& legend, Matrix position, bool box, double lineWidth, Matrix backgroundColor
) const
{
  // See https://github.com/plotly/plotly.js/issues/1668
  // Position the legend in the right place
  legend["x"] = position(0);
  legend["y"] = position(1);

  legend["xanchor"] = "left";
  legend["yanchor"] = "bottom";

  legend["xref"] = "paper";
  legend["yref"] = "paper";

  // Add border to the legend
  if (box)
  {
    legend["bordercolor"] = "rgb(0,0,0)";
    legend["borderwidth"] = lineWidth;
  }
  else
  {
    legend["borderwidth"] = 0;
  }

  // Set legend background color
  legend["bgcolor"] = matrix2rgb(backgroundColor);
}

void plotly_graphics_toolkit::line(
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
) const
{
  line["type"] = type;
  line["visibility"] = visible;
  // TODO: marker color
  line["marker"]["line"]["color"] = line["marker"]["color"] = line["line"]["color"] = matrix2rgb(lineColor);

  if (type == "scatter3d")
  {
    line["line"]["width"] = std::max(1.5, lineWidth);
    line["marker"]["size"] = markerSize / 3;
  }
  else
  {
    line["line"]["width"] = lineWidth;
    line["marker"]["size"] = markerSize;
  }

  // Set line type
  if (marker != "none" && lineStyle != "none")
    line["mode"] = "lines+markers";
  else if (marker != "none")
    line["mode"] = "markers";
  else if (lineStyle != "none")
    line["mode"] = "lines";
  else
    line["mode"] = "none";

  // Set line style
  if (lineStyle == "-")
    line["line"]["dash"] = "solid";
  else if (lineStyle == "--")
    line["line"]["dash"] = "dash";
  else if (lineStyle == ":")
    line["line"]["dash"] = "dot";
  else if (lineStyle == "-.")
    line["line"]["dash"] = "dashdot";

  // Set marker style
  if ((marker == ".") || (marker == "o"))
    line["marker"]["symbol"] = "circle";
  else if (marker == "x")
    line["marker"]["symbol"] = "x-thin-open";
  else if (marker == "+")
    line["marker"]["symbol"] = "cross-thin-open";
  else if (marker == "*")
    line["marker"]["symbol"] = "asterisk-open";
  else if (marker == "s" || marker == "square")
    line["marker"]["symbol"] = "square";
  else if (marker == "d" || marker == "diamond")
    line["marker"]["symbol"] = "diamond";
  else if (marker == "v")
    line["marker"]["symbol"] = "triangle-down";
  else if (marker == "^")
    line["marker"]["symbol"] = "triangle-up";
  else if (marker == "<")
    line["marker"]["symbol"] = "triangle-left";
  else if (marker == ">")
    line["marker"]["symbol"] = "triangle-right";
  else if (marker == "p" || marker == "pentagram")
    line["marker"]["symbol"] = "star";
  else if (marker == "h" || marker == "hexagram")
    line["marker"]["symbol"] = "hexagram";
  else  // Otherwise use plotly markers
    line["marker"]["symbol"] = marker;

  // Filling data
  if (type == "scatterpolar")
  {
    // In polar charts the points are in XY coordinates
    // so we need to convert them in polar coordinates
    // by ourselves
    for (octave_idx_type i = 0; i < xdata.cols(); i++)
    {
      auto const vector = std::complex<double>(xdata(i), ydata(i));
      auto const ui = static_cast<std::size_t>(i);
      line["r"][ui] = std::abs(vector);
      line["theta"][ui] = std::arg(vector);
    }
    line["thetaunit"] = "radians";
  }
  else
  {
    for (octave_idx_type i = 0; i < xdata.cols(); i++)
    {
      auto const ui = static_cast<std::size_t>(i);
      line["x"][ui] = xdata(i);
    }
    for (octave_idx_type i = 0; i < ydata.cols(); i++)
    {
      auto const ui = static_cast<std::size_t>(i);
      line["y"][ui] = ydata(i);
    }
    for (octave_idx_type i = 0; i < zdata.cols(); i++)
    {
      auto const ui = static_cast<std::size_t>(i);
      line["z"][ui] = zdata(i);
    }
  }
}

void plotly_graphics_toolkit::surface(
  nl::json& surf, bool visible, Matrix xdata, Matrix ydata, Matrix zdata, Matrix cdata, Matrix colorMap, Matrix clim
) const
{
  surf["type"] = "surface";
  surf["visibility"] = visible;

  for (octave_idx_type i = 0; i < xdata.cols(); i++)
  {
    auto const ui = static_cast<std::size_t>(i);
    surf["x"][ui] = xdata(0, i);
  }

  for (octave_idx_type j = 0; j < ydata.rows(); j++)
  {
    auto const uj = static_cast<std::size_t>(j);
    surf["y"][uj] = ydata(j, 0);
  }

  for (octave_idx_type i = 0; i < zdata.rows(); i++)
  {
    auto const ui = static_cast<std::size_t>(i);
    for (octave_idx_type j = 0; j < zdata.cols(); j++)
    {
      auto const uj = static_cast<std::size_t>(j);
      surf["z"][ui][uj] = zdata(i, j);
    }
  }

  for (octave_idx_type i = 0; i < cdata.rows(); i++)
  {
    auto const ui = static_cast<std::size_t>(i);
    for (octave_idx_type j = 0; j < cdata.cols(); j++)
    {
      auto const uj = static_cast<std::size_t>(j);
      surf["surfacecolor"][ui][uj] = cdata(i, j);
    }
  }

  for (octave_idx_type i = 0; i < colorMap.rows(); i++)
  {
    auto const ui = static_cast<std::size_t>(i);
    surf["colorscale"][ui][0] = static_cast<double>(i) / static_cast<double>(colorMap.rows() - 1);
    surf["colorscale"][ui][1] = matrix2rgb(colorMap.row(i));
  }

  // Setting common colorscale
  surf["showscale"] = false;
  surf["autocolorscale"] = "false";
  surf["cauto"] = "false";
  surf["cmin"] = clim(0);
  surf["cmax"] = clim(1);

  // Setting contour lines
  surf["contours"]["x"]["show"] = false;
  surf["contours"]["y"]["show"] = false;
  surf["contours"]["z"]["show"] = false;
  surf["contours"]["x"]["highlight"] = true;
  surf["contours"]["y"]["highlight"] = true;
  surf["contours"]["z"]["highlight"] = true;
  surf["contours"]["x"]["highlightcolor"] = "rgb(0,0,0)";
  surf["contours"]["y"]["highlightcolor"] = "rgb(0,0,0)";
  surf["contours"]["z"]["highlightcolor"] = "rgb(0,0,0)";
  surf["contours"]["x"]["highlightwidth"] = 1;
  surf["contours"]["y"]["highlightwidth"] = 1;
  surf["contours"]["z"]["highlightwidth"] = 1;
}

void plotly_graphics_toolkit::setLegendVisibility(nl::json& data, std::string name) const
{
  // Configuring name and visibility in the legend
  if (name != "")
  {
    data["name"] = name;
    data["showlegend"] = true;
  }
  else
  {
    data["name"] = "";
    data["showlegend"] = false;
  }
}

void register_all(octave::interpreter& interpreter)
{
  // Install the toolkit into the interpreter
  interpreter.get_gtk_manager().register_toolkit("plotly");
  interpreter.get_gtk_manager().load_toolkit(octave::graphics_toolkit(new plotly_graphics_toolkit(interpreter)));
}

}  // namespace xeus_octave::tk::plotly
