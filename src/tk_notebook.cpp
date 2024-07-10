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

#include "xeus-octave/opengl.hpp"

#include <GLFW/glfw3.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <octave/gl-render.h>
#include <octave/graphics-toolkit.h>
#include <octave/graphics.h>
#include <octave/interpreter.h>
#include <octave/ov.h>
#include <png.h>
#include <xeus/xbase64.hpp>

#include "xeus-octave/plotstream.hpp"
#include "xeus-octave/tk_notebook.hpp"
#include "xeus-octave/xinterpreter.hpp"

namespace nl = nlohmann;
namespace oc = octave;
using namespace std::chrono;

namespace xeus_octave::tk::notebook
{

namespace
{

/**
 * Encode a set of opengl pixels to a PNG stream stored in a std::vector
 */
auto png_encode(std::vector<unsigned char>& data, unsigned int width, unsigned int height)
{
  // A RAII structure to manage the lifetime of PNG structures
  struct PngManager
  {
    png_structp png;
    png_infop info;

    PngManager()
    {
      png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
      info = png_create_info_struct(png);
    }

    ~PngManager() { png_destroy_write_struct(&png, &info); }
  };

  auto m = PngManager();

  setjmp(png_jmpbuf(m.png));

  png_set_IHDR(
    m.png,
    m.info,
    width,
    height,
    8,
    PNG_COLOR_TYPE_RGB,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );

  std::vector<unsigned char*> rows(height);
  for (std::size_t y = 0; y < height; y++)
    rows[height - 1 - y] = data.data() + y * width * 3;
  png_set_rows(m.png, m.info, &rows[0]);

  std::vector<char> out;

  png_set_write_fn(
    m.png,
    &out,
    [](png_structp png_, png_bytep d, png_size_t l)
    {
      std::vector<char>* img_ptr = static_cast<std::vector<char>*>(png_get_io_ptr(png_));
      img_ptr->insert(img_ptr->end(), d, d + l);
    },
    nullptr
  );

  png_write_png(m.png, m.info, PNG_TRANSFORM_IDENTITY, NULL);

  return out;
}

}  // namespace

glfw_graphics_toolkit::glfw_graphics_toolkit(std::string const& nm) : octave::base_graphics_toolkit(nm)
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

  GLFWwindow* window = glfwCreateWindow(1, 1, "", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return;
  }

  glfwMakeContextCurrent(window);

  gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

#ifndef NDEBUG
  std::clog << "OpenGL vendor: " << glGetString(GL_VENDOR) << '\n';
  std::clog << "OpenGL renderer: " << glGetString(GL_RENDERER) << '\n';
  std::clog << "OpenGL version: " << glGetString(GL_VERSION) << '\n';
#endif

  glfwDestroyWindow(window);
}

glfw_graphics_toolkit::~glfw_graphics_toolkit()
{
  glfwTerminate();
}

bool glfw_graphics_toolkit::initialize(octave::graphics_object const& go)
{
  // We use this call for initializing only the figure
  if (go.isa("figure"))
  {
    // Set the pixel ratio
    auto& figureProperties = dynamic_cast<oc::figure::properties&>(oc::graphics_object(go).get_properties());

    // Get monitor scale
    float xscale, yscale;

    if (auto* monitor = glfwGetPrimaryMonitor())
      glfwGetMonitorContentScale(monitor, &xscale, &yscale);
    else
      xscale = yscale = 1;

    float dpr = std::max(xscale, yscale);

#ifndef NDEBUG
    std::clog << "Device pixel ratio: " << dpr << '\n';
#endif

    // Store it in the figure
    figureProperties.set___device_pixel_ratio__(dpr);

    return true;
  }

  return false;
}

void glfw_graphics_toolkit::redraw_figure(octave::graphics_object const& go) const
{
#ifndef NDEBUG
  auto start = high_resolution_clock::now();
#endif

  // Get width height and scale factor
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

  // Use the octave renderer to draw the plot on the EGL context
  oc::opengl_functions m_glfcns;
  oc::opengl_renderer m_renderer(m_glfcns);

  // Create a hidden GLFW window
  auto window = glfwCreateWindow(width, height, "", NULL, NULL);
  glfwMakeContextCurrent(window);

  // Render
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

  // Get pixels
  auto screen = std::vector<unsigned char>(uwidth * uheight * 3);
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, screen.data());

#ifndef NDEBUG
  auto encode_start = high_resolution_clock::now();
#endif
  auto img = png_encode(screen, uwidth, uheight);
#ifndef NDEBUG
  auto encode_stop = high_resolution_clock::now();
  auto encode_duration = duration_cast<microseconds>(encode_stop - encode_start);
  std::clog << "Encode time: " << encode_duration.count() << '\n';
#endif
#ifndef NDEBUG
  auto send_start = high_resolution_clock::now();
#endif

  send_figure(go, img, width, height, dpr);

#ifndef NDEBUG
  auto send_stop = high_resolution_clock::now();
  auto send_duration = duration_cast<microseconds>(send_stop - send_start);
  std::clog << "Send time: " << send_duration.count() << '\n';
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  std::clog << "Draw time: " << duration.count() << '\n';
#endif

  // Destroy window
  glfwDestroyWindow(window);
}

bool notebook_graphics_toolkit::initialize(oc::graphics_object const& go)
{
  bool ret = glfw_graphics_toolkit::initialize(go);

  if (go.isa("figure"))
  {
    // Create a new object id and store it in the figure
    setPlotStream(go, xeus::new_xguid());

    // Request to show the empty figure (this serves as a placeholder to
    // keep the output order, as the figures are drawn after the last line
    // is executed)
    show_figure(go);

    return true;
  }

  return ret;
}

void notebook_graphics_toolkit::show_figure(oc::graphics_object const& go) const
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

void notebook_graphics_toolkit::send_figure(
  oc::graphics_object const& go, std::vector<char> const& img, int width, int height, double dpr
) const
{
  // Retrieve the figure id
  std::string id = getPlotStream<std::string>(go);

  nl::json data, meta, tran;

  data["image/png"] = xeus::base64encode(std::string(img.begin(), img.end()));
  // Send real width and height through metadata for optimal scaling
  meta["image/png"] = {
    {"width", width / dpr},
    {"height", height / dpr},
  };
  // Dislplay id for updating existing display
  tran["display_id"] = id;

  // Update
  dynamic_cast<xoctave_interpreter&>(xeus::get_interpreter()).update_display_data(data, meta, tran);
}

void register_all(octave::interpreter& interpreter)
{
  // Install the toolkit into the interpreter
  interpreter.get_gtk_manager().register_toolkit("notebook");
  interpreter.get_gtk_manager().load_toolkit(octave::graphics_toolkit(new notebook_graphics_toolkit()));
}

}  // namespace xeus_octave::tk::notebook
