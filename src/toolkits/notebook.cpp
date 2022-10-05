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

#ifdef XEUS_OCTAVE_NOTEBOOK_TOOLKIT_ENABLED

#include <algorithm>
#include <chrono>
#include <cmath>
#include <csetjmp>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <ostream>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#ifdef XEUS_OCTAVE_GLFW3_OSMESA_BACKEND
#define GLFW_EXPOSE_NATIVE_OSMESA
#include <GLFW/glfw3native.h>
#endif
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <octave/gl-render.h>
#include <octave/graphics-toolkit.h>
#include <octave/graphics.h>
#include <octave/interpreter.h>
#include <octave/ov.h>
#include <png.h>
#include <xtl/xbase64.hpp>

#include "xeus-octave/xinterpreter.hpp"

#include "notebook.hpp"
#include "opengl.hpp"
#include "plotstream.hpp"

namespace nl = nlohmann;
namespace oc = octave;
using namespace std::chrono;

namespace xeus_octave
{

notebook_graphics_toolkit::notebook_graphics_toolkit(oc::interpreter& interpreter) :
  base_graphics_toolkit("notebook"), m_interpreter(interpreter)
{
  glfwSetErrorCallback([](int error, char const* description)
                       { std::clog << "GLFW Error: " << description << " (" << error << ")" << '\n'; });

  glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);

  if (!glfwInit())
  {
    std::clog << "Cannot initialize GLFW" << '\n';
    return;
  }

  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#ifndef XEUS_OCTAVE_GLFW3_OSMESA_BACKEND
  window = glfwCreateWindow(100, 100, "", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return;
  }

  glfwMakeContextCurrent(window);

#ifndef NDEBUG
  std::clog << "OpenGL vendor: " << glGetString(GL_VENDOR) << '\n';
#endif

#endif
}

notebook_graphics_toolkit::~notebook_graphics_toolkit()
{
#ifndef XEUS_OCTAVE_GLFW3_OSMESA_BACKEND
  if (window)
    glfwDestroyWindow(window);
#endif

  glfwTerminate();
}

bool notebook_graphics_toolkit::initialize(oc::graphics_object const& go)
{
  if (go.isa("figure"))
  {
    // Set the pixel ratio
    auto& figureProperties = dynamic_cast<oc::figure::properties&>(oc::graphics_object(go).get_properties());
    float xscale, yscale;

    if (auto* monitor = glfwGetPrimaryMonitor())
      glfwGetMonitorContentScale(monitor, &xscale, &yscale);
    else
      xscale = yscale = 1;

    float dpr = std::max(xscale, yscale);

#ifndef NDEBUG
    std::clog << "Device pixel ratio: " << dpr << '\n';
#endif

    figureProperties.set___device_pixel_ratio__(dpr);

    setPlotStream(go, rand());
    show_figure(go);

    return true;
  }

  return false;
}

void notebook_graphics_toolkit::finalize(oc::graphics_object const&) {}

void notebook_graphics_toolkit::show_figure(oc::graphics_object const& go) const
{
  int id = getPlotStream(go);

  auto tran = nl::json::object();
  tran["display_id"] = id;
  dynamic_cast<xoctave_interpreter&>(xeus::get_interpreter())
    .display_data(nl::json::object(), nl::json::object(), tran);
}

void notebook_graphics_toolkit::redraw_figure(oc::graphics_object const& go) const
{
#ifndef NDEBUG
  std::clog << "------------" << '\n';
  auto start = high_resolution_clock::now();
#endif

  int id = getPlotStream(go);
  auto& figureProperties = dynamic_cast<oc::figure::properties&>(oc::graphics_object(go).get_properties());
  Matrix figurePosition = figureProperties.get_position().matrix_value();

  auto const dpr = figureProperties.get___device_pixel_ratio__();
  auto const int_cast = [](double x) -> int
  {
    using limits = std::numeric_limits<int>;
    assert((limits::min() <= x) && (x <= limits::max()));
    return static_cast<int>(std::lround(x));
  };
  auto const width = int_cast(figurePosition(2) * dpr);
  auto const height = int_cast(figurePosition(3) * dpr);
  assert(width >= 0);
  auto const uwidth = static_cast<unsigned int>(width);
  assert(width >= 0);
  auto const uheight = static_cast<unsigned int>(height);

  oc::opengl_functions m_glfcns;
  oc::opengl_renderer m_renderer(m_glfcns);

#ifdef XEUS_OCTAVE_GLFW3_OSMESA_BACKEND
  auto window = glfwCreateWindow(width, height, "", NULL, NULL);
  glfwMakeContextCurrent(window);
#else
  glfwSetWindowSize(window, width, height);
#endif

  m_renderer.set_viewport(width, height);
  m_renderer.set_device_pixel_ratio(dpr);
#ifndef NDEBUG
  auto render_start = high_resolution_clock::now();
#endif

  m_renderer.draw(go);

#ifndef NDEBUG
  auto render_stop = high_resolution_clock::now();
  auto render_duration = duration_cast<microseconds>(render_stop - render_start);
  std::clog << "Render time: " << render_duration.count() << '\n';
#endif

  auto screen = std::vector<unsigned char>(uwidth * uheight * 3);
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, screen.data());

#ifndef NDEBUG
  auto encode_start = high_resolution_clock::now();
#endif

  png_structp p = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  png_infop i = png_create_info_struct(p);

  setjmp(png_jmpbuf(p));

  png_set_IHDR(
    p,
    i,
    uwidth,
    uheight,
    8,
    PNG_COLOR_TYPE_RGB,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );

  std::vector<unsigned char*> rows(uheight);
  for (std::size_t y = 0; y < uheight; y++)
    rows[uheight - 1 - y] = screen.data() + y * uwidth * 3;
  png_set_rows(p, i, &rows[0]);

  auto const img = [](auto& png)
  {
    auto out = std::string();
    png_set_write_fn(
      png,
      &out,
      [](png_structp png_, png_bytep d, png_size_t l)
      {
        std::string* out_ = static_cast<std::string*>(png_get_io_ptr(png_));
        out_->insert(out_->end(), d, d + l);
      },
      nullptr
    );
    return out;
  }(p);

  png_write_png(p, i, PNG_TRANSFORM_IDENTITY, NULL);

  png_destroy_write_struct(&p, &i);

#ifndef NDEBUG
  auto encode_stop = high_resolution_clock::now();
  auto encode_duration = duration_cast<microseconds>(encode_stop - encode_start);
  std::clog << "Encode time: " << encode_duration.count() << '\n';
#endif
#ifndef NDEBUG
  auto send_start = high_resolution_clock::now();
#endif

  nl::json data, meta, tran;

  data["image/png"] = xtl::base64encode(img);
  meta["image/png"] = {{"width", width / dpr}, {"height", height / dpr}};
  tran["display_id"] = id;

  dynamic_cast<xoctave_interpreter&>(xeus::get_interpreter()).update_display_data(data, meta, tran);

#ifndef NDEBUG
  auto send_stop = high_resolution_clock::now();
  auto send_duration = duration_cast<microseconds>(send_stop - send_start);
  std::clog << "Send time: " << send_duration.count() << '\n';
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  std::clog << "Draw time: " << duration.count() << '\n';
#endif

#ifdef XEUS_OCTAVE_GLFW3_OSMESA_BACKEND
  glfwDestroyWindow(window);
#endif
}

void notebook_graphics_toolkit::update(oc::graphics_object const&, int) {}

}  // namespace xeus_octave

#endif
