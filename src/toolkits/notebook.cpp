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

#include "notebook.hpp"

#include "config.h"

#ifdef NOTEBOOK_TOOLKIT_ENABLED

#include <GLFW/glfw3.h>

#ifdef NOTEBOOK_TOOLKIT_CPU
#define GLFW_EXPOSE_NATIVE_OSMESA
#include <GLFW/glfw3native.h>
#endif

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
#include <nlohmann/json_fwd.hpp>
#include <ostream>
#include <string>

#include "plotstream.hpp"
#include "xoctave_interpreter.hpp"
#include "xtl/xbase64.hpp"

using namespace std::chrono;
using namespace nlohmann;

namespace xoctave {

notebook_graphics_toolkit::notebook_graphics_toolkit(octave::interpreter& interpreter) : base_graphics_toolkit("notebook"), m_interpreter(interpreter) {
	glfwSetErrorCallback([](int error, const char* description) {
		std::clog << "GLFW Error: " << description << " (" << error << ")" << std::endl;
	});

	glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);

	if (!glfwInit()) {
		std::clog << "Cannot initialize GLFW" << std::endl;
		return;
	}

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#ifndef NOTEBOOK_TOOLKIT_CPU
	window = glfwCreateWindow(100, 100, "", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);

#ifndef NDEBUG
	std::clog << "OpenGL vendor: " << glGetString(GL_VENDOR) << std::endl;
#endif

#endif
}

notebook_graphics_toolkit::~notebook_graphics_toolkit() {
#ifndef NOTEBOOK_TOOLKIT_CPU
	if (window)
		glfwDestroyWindow(window);
#endif

	glfwTerminate();
}

bool notebook_graphics_toolkit::initialize(const octave::graphics_object& go) {
	if (go.isa("figure")) {
		// Set the pixel ratio
		figure::properties& figureProperties = dynamic_cast<figure::properties&>(octave::graphics_object(go).get_properties());
		float xscale, yscale;

		if (auto* monitor = glfwGetPrimaryMonitor())
			glfwGetMonitorContentScale(monitor, &xscale, &yscale);
		else
			xscale = yscale = 1;

		float dpr = std::max(xscale, yscale);

#ifndef NDEBUG
		std::clog << "Device pixel ratio: " << dpr << std::endl;
#endif

		figureProperties.set___device_pixel_ratio__(dpr);

		setPlotStream(go, rand());
		show_figure(go);

		return true;
	}

	return false;
}

void notebook_graphics_toolkit::finalize(const octave::graphics_object&) {
}

void notebook_graphics_toolkit::show_figure(const octave::graphics_object& go) const {
	int id = getPlotStream(go);

	json tran;
	tran["display_id"] = id;
	dynamic_cast<xoctave_interpreter&>(xeus::get_interpreter()).display_data(json::object(), json::object(), tran);
}

void notebook_graphics_toolkit::redraw_figure(const octave::graphics_object& go) const {
#ifndef NDEBUG
	std::clog << "------------" << std::endl;
	auto start = high_resolution_clock::now();
#endif

	int id = getPlotStream(go);
	figure::properties& figureProperties = dynamic_cast<figure::properties&>(octave::graphics_object(go).get_properties());
	Matrix figurePosition = figureProperties.get_position().matrix_value();

	double dpr = figureProperties.get___device_pixel_ratio__();

	int width = figurePosition(2) * dpr;
	int height = figurePosition(3) * dpr;

	octave::opengl_functions m_glfcns;
	octave::opengl_renderer m_renderer(m_glfcns);

#ifdef NOTEBOOK_TOOLKIT_CPU
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
	std::clog << "Render time: " << render_duration.count() << std::endl;
#endif

	unsigned char* screen;

	screen = new unsigned char[width * height * 3];
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, screen);

#ifndef NDEBUG
	auto encode_start = high_resolution_clock::now();
#endif

	std::string out;
	png_structp p = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	png_infop i = png_create_info_struct(p);

	setjmp(png_jmpbuf(p));

	png_set_IHDR(p, i, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	std::vector<unsigned char*> rows(height);
	for (int y = 0; y < height; y++)
		rows[height - 1 - y] = screen + y * width * 3;
	png_set_rows(p, i, &rows[0]);

	png_set_write_fn(
		p, &out, [](png_structp p, png_bytep d, png_size_t l) {
			std::string* out = static_cast<std::string*>(png_get_io_ptr(p));
			out->insert(out->end(), d, d + l);
		},
		nullptr);

	png_write_png(p, i, PNG_TRANSFORM_IDENTITY, NULL);

	png_destroy_write_struct(&p, &i);

#ifndef NDEBUG
	auto encode_stop = high_resolution_clock::now();
	auto encode_duration = duration_cast<microseconds>(encode_stop - encode_start);
	std::clog << "Encode time: " << encode_duration.count() << std::endl;
#endif
#ifndef NDEBUG
	auto send_start = high_resolution_clock::now();
#endif

	json data, meta, tran;

	data["image/png"] = xtl::base64encode(out);
	meta["image/png"] = {
		{"width", width / dpr},
		{"height", height / dpr}};
	tran["display_id"] = id;

	dynamic_cast<xoctave_interpreter&>(xeus::get_interpreter()).update_display_data(data, meta, tran);

#ifndef NDEBUG
	auto send_stop = high_resolution_clock::now();
	auto send_duration = duration_cast<microseconds>(send_stop - send_start);
	std::clog << "Send time: " << send_duration.count() << std::endl;
#endif

	delete[] screen;

#ifndef NDEBUG
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	std::clog << "Draw time: " << duration.count() << std::endl;
#endif

#ifdef NOTEBOOK_TOOLKIT_CPU
	glfwDestroyWindow(window);
#endif
}

void notebook_graphics_toolkit::update(const octave::graphics_object&, int) {
}

}  // namespace xoctave

#endif
