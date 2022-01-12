/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#include "xeus-octave/tk_notebook.hpp"

// Prevent include reordering
#include "xeus-octave/opengl.hpp"
// Prevent include reordering
#include <GLFW/glfw3.h>
#include <octave/gl-render.h>
#include <octave/graphics-toolkit.h>
#include <octave/graphics.h>
#include <octave/interpreter.h>
#include <octave/ov.h>
#include <png.h>

#include <algorithm>
#include <chrono>
#include <csetjmp>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <ostream>
#include <string>

#include "xeus-octave/plotstream.hpp"
#include "xeus-octave/xinterpreter.hpp"
#include "xeus/xguid.hpp"
#include "xeus/xinterpreter.hpp"
#include "xwidgets/ximage.hpp"

namespace xeus_octave::tk::notebook {
notebook_graphics_toolkit::notebook_graphics_toolkit(octave::interpreter& interpreter) : base_graphics_toolkit("notebook"), m_interpreter(interpreter) {
	glfwSetErrorCallback([](int error, const char* description) {
		std::clog << "GLFW Error: " << description << " (" << error << ")" << std::endl;
	});

	glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);

#ifdef __linux__
	const char* wayland = std::getenv("WAYLAND_DISPLAY");
	const char* x11 = std::getenv("DISPLAY");

	// Select the correct display manager
	if (wayland && strcmp(wayland, "") && glfwPlatformSupported(GLFW_PLATFORM_WAYLAND))
		glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
	else if (x11 && strcmp(x11, ""))
		glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
	else
		glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_NULL);
#endif

	if (!glfwInit()) {
		std::clog << "Cannot initialize GLFW" << std::endl;
		return;
	}

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(1, 1, "", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

#if (GLFW_VERSION_MAJOR >= 3) && (GLFW_VERSION_MINOR >= 4)
	std::clog << "GLFW Platform: ";

	switch (glfwGetPlatform()) {
		case GLFW_PLATFORM_WIN32:
			std::clog << "WIN32" << std::endl;
			break;
		case GLFW_PLATFORM_COCOA:
			std::clog << "COCOA" << std::endl;
			break;
		case GLFW_PLATFORM_WAYLAND:
			std::clog << "WAYLAND" << std::endl;
			break;
		case GLFW_PLATFORM_X11:
			std::clog << "X11" << std::endl;
			break;
		case GLFW_PLATFORM_NULL:
			std::clog << "NULL" << std::endl;
			break;
	}
#endif

	std::clog << "OpenGL vendor: " << glGetString(GL_VENDOR) << std::endl;

	glfwDestroyWindow(window);
}

notebook_graphics_toolkit::~notebook_graphics_toolkit() {
	glfwTerminate();
}

bool notebook_graphics_toolkit::initialize(const graphics_object& go) {
	// We use this call for initializing only the figure
	if (go.isa("figure")) {
		// Set the pixel ratio
		figure::properties& figureProperties = dynamic_cast<figure::properties&>(graphics_object(go).get_properties());

		// Get monitor scale
		float xscale, yscale;

		if (auto* monitor = glfwGetPrimaryMonitor())
			glfwGetMonitorContentScale(monitor, &xscale, &yscale);
		else
			// Force double scale if no monitor is detected (hidpi will look good)
			xscale = yscale = 2;

		float dpr = std::max(xscale, yscale);

		// Store it in the figure
		figureProperties.set___device_pixel_ratio__(dpr);

		xw::image* figure = new xw::image;

		// Create a new object id and store it in the figure
		setPlotStream(go, figure);

		// Request to show the empty figure (this serves as a placeholder to
		// keep the output order, as the figures are drawn after the last line
		// is executed)
		show_figure(go);

		return true;
	}

	return false;
}

void notebook_graphics_toolkit::finalize(const graphics_object& go) {
	// Get an unique identifier for this object, to be used as a display id
	// in the display_data request for subsequent updates of the plot
	xw::image* figure = getPlotStream(go);

	delete figure;
}

void notebook_graphics_toolkit::show_figure(const graphics_object& go) const {
	// Get an unique identifier for this object, to be used as a display id
	// in the display_data request for subsequent updates of the plot
	xw::image* figure = getPlotStream(go);

	// Display an empty figure (this is equivalent to the action of creating)
	// a window, and prepares a display with the correct display_id for
	// future updates
	figure->display();
}

void notebook_graphics_toolkit::redraw_figure(const graphics_object& go) const {
	// Retrieve the figure id
	xw::image* figure = getPlotStream(go);

	// Get width height and scale factor
	figure::properties& figureProperties = dynamic_cast<figure::properties&>(graphics_object(go).get_properties());
	Matrix figurePosition = figureProperties.get_position().matrix_value();
	double dpr = figureProperties.get___device_pixel_ratio__();
	int width = figurePosition(2) * dpr;
	int height = figurePosition(3) * dpr;

	// Use the octave renderer to draw the plot on the EGL context
	octave::opengl_functions m_glfcns;
	octave::opengl_renderer m_renderer(m_glfcns);

	// Create a hidden GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, "", NULL, NULL);
	glfwMakeContextCurrent(window);

	// Render
	m_renderer.set_viewport(width, height);
	m_renderer.set_device_pixel_ratio(dpr);
	m_renderer.draw(go);

	// Get pixels
	unsigned char* screen = new unsigned char[width * height * 3];
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, screen);

	// Encode as PNG
	std::vector<char> out;
	std::vector<unsigned char*> rows(height);
	png_structp p = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	png_infop i = png_create_info_struct(p);
	setjmp(png_jmpbuf(p));
	png_set_IHDR(p, i, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	for (int y = 0; y < height; y++)
		rows[height - 1 - y] = screen + y * width * 3;
	png_set_rows(p, i, &rows[0]);
	png_set_write_fn(
		p, &out, [](png_structp p, png_bytep d, png_size_t l) {
			std::vector<char>* out = static_cast<std::vector<char>*>(png_get_io_ptr(p));
			out->insert(out->end(), d, d + l);
		},
		nullptr);
	png_write_png(p, i, PNG_TRANSFORM_IDENTITY, NULL);
	png_destroy_write_struct(&p, &i);
	delete[] screen;

	// TODO check value is changed before setting
	figure->width = std::to_string(width / dpr);
	figure->height = std::to_string(height / dpr);
	figure->value = out;

	// Destroy window
	glfwDestroyWindow(window);
}

void notebook_graphics_toolkit::update(const graphics_object&, int) {
	// Unused
}

void register_all(octave::interpreter& interpreter) {
	// Install the toolkit into the interpreter
	interpreter.get_gtk_manager().register_toolkit("notebook");
	interpreter.get_gtk_manager().load_toolkit(
		octave::graphics_toolkit(
			new notebook_graphics_toolkit(interpreter)));
}
}  // namespace xeus_octave::tk::notebook