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

#include <algorithm>
#include <chrono>
#include <cppcodec/base64_rfc4648.hpp>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <ostream>

#include "lodepng.h"
#include "plotstream.hpp"
#include "xoctave_interpreter.hpp"

using namespace std::chrono;
using namespace nlohmann;

using base64 = cppcodec::base64_rfc4648;

namespace xoctave {

notebook_graphics_toolkit::notebook_graphics_toolkit(octave::interpreter& interpreter) : base_graphics_toolkit("notebook"), m_interpreter(interpreter) {
    glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);

	if (!glfwInit()) {
		std::clog << "Cannot initialize GLFW" << std::endl;
		return;
	}

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	window = glfwCreateWindow(100, 100, "", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);

#ifndef NDEBUG
	std::clog << "OpenGL vendor: " << glGetString(GL_VENDOR) << std::endl;
#endif
}

notebook_graphics_toolkit::~notebook_graphics_toolkit() {
	if (window)
		glfwDestroyWindow(window);

	glfwTerminate();
}

bool notebook_graphics_toolkit::initialize(const graphics_object& go) {
	if (go.isa("figure")) {
		// Set the pixel ratio
		figure::properties& figureProperties = dynamic_cast<figure::properties&>(graphics_object(go).get_properties());
		float xscale, yscale;

		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xscale, &yscale);

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

void notebook_graphics_toolkit::finalize(const graphics_object&) {
}

void notebook_graphics_toolkit::show_figure(const graphics_object& go) const {
	int id = getPlotStream(go);

	json tran;
	tran["display_id"] = id;
	dynamic_cast<xoctave_interpreter&>(xeus::get_interpreter()).display_data(json::object(), json::object(), tran);
}

void notebook_graphics_toolkit::redraw_figure(const graphics_object& go) const {
#ifndef NDEBUG
	std::clog << "------------" << std::endl;
	auto start = high_resolution_clock::now();
#endif

	int id = getPlotStream(go);
	figure::properties& figureProperties = dynamic_cast<figure::properties&>(graphics_object(go).get_properties());
	Matrix figurePosition = figureProperties.get_position().matrix_value();

	double dpr = figureProperties.get___device_pixel_ratio__();

	int width = figurePosition(2) * dpr;
	int height = figurePosition(3) * dpr;

	octave::opengl_functions m_glfcns;
	octave::opengl_renderer m_renderer(m_glfcns);

	glfwSetWindowSize(window, width, height);

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

#ifndef NDEBUG
	auto copy_start = high_resolution_clock::now();
#endif

	std::vector<unsigned char> out;

	unsigned char* screen;
	unsigned char* flip = new unsigned char[width * height * 3];

#ifdef NOTEBOOK_TOOLKIT_CPU
	glfwGetOSMesaColorBuffer(window, &width, &height, NULL, (void**) &screen);
#else
	screen = new unsigned char[width * height * 3];
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, screen);
#endif

	for (int i = 0; i < height; i++) {
		std::memcpy(&flip[(height - i - 1) * width * 3], &screen[i * width * 3], width * 3);
	}
#ifndef NDEBUG
	auto copy_stop = high_resolution_clock::now();
	auto copy_duration = duration_cast<microseconds>(copy_stop - copy_start);
	std::clog << "Copy time: " << copy_duration.count() << std::endl;
#endif

#ifndef NDEBUG
	auto encode_start = high_resolution_clock::now();
#endif

	lodepng::encode(out, flip, width, height, LCT_RGB);
#ifndef NDEBUG
	auto encode_stop = high_resolution_clock::now();
	auto encode_duration = duration_cast<microseconds>(encode_stop - encode_start);
	std::clog << "Encode time: " << encode_duration.count() << std::endl;
#endif
#ifndef NDEBUG
	auto send_start = high_resolution_clock::now();
#endif

	json data, meta, tran;

	data["image/png"] = base64::encode(out);
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

#ifndef NOTEBOOK_TOOLKIT_CPU
	delete[] screen;
#endif
	delete[] flip;

#ifndef NDEBUG
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	std::clog << "Draw time: " << duration.count() << std::endl;
#endif
}

void notebook_graphics_toolkit::update(const graphics_object&, int) {
}

}  // namespace xoctave

#endif
