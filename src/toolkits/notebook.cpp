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

#include <octave/graphics-toolkit.h>
#include <octave/graphics.h>
#include <octave/interpreter.h>
#include <octave/ov.h>

#include <chrono>
#include <cppcodec/base64_rfc4648.hpp>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <ostream>
#include <thread>
#include <utility>

#include "config.h"
#include "lodepng.h"
#include "plotstream.hpp"
#include "xoctave_interpreter.hpp"

using namespace std::chrono;
using namespace nlohmann;

using base64 = cppcodec::base64_rfc4648;

namespace xoctave {

notebook_graphics_toolkit::notebook_graphics_toolkit(octave::interpreter& interpreter) : base_graphics_toolkit("notebook"),
#if defined(Qt5Gui_FOUND)
																						 t_renderer(std::bind(&notebook_graphics_toolkit::renderer, this)),
#endif
																						 m_interpreter(interpreter) {
#if defined(Qt5Gui_FOUND)
#elif defined(X11_FOUND)
	Display* dpy;
	Window root;
	GLint attr[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
	XVisualInfo* vi;
	GLXContext glc;

	/* open display */
	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "cannot connect to X server\n\n");
		exit(1);
	}

	/* get root window */
	root = DefaultRootWindow(dpy);

	/* get visual matching attr */
	if (!(vi = glXChooseVisual(dpy, 0, attr))) {
		fprintf(stderr, "no appropriate visual found\n\n");
		exit(1);
	}

	/* create a context using the root window */
	if (!(glc = glXCreateContext(dpy, vi, NULL, GL_TRUE))) {
		fprintf(stderr, "failed to create context\n\n");
		exit(1);
	}
	glXMakeCurrent(dpy, root, glc);
#elif defined(osmesa_FOUND)
	OSMesaContext osc;

	osc = OSMesaCreateContext(OSMESA_RGBA, NULL);
	OSMesaMakeCurrent(osc, screen, GL_UNSIGNED_BYTE, 2000, 2000);
#endif
}

bool notebook_graphics_toolkit::initialize(const graphics_object& go) {
	if (go.isa("figure")) {
		// Set the pixel ratio
		figure::properties& figureProperties = dynamic_cast<figure::properties&>(graphics_object(go).get_properties());
		double dpr = backend.get_dpr();

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

#ifndef NDEBUG
	auto render_start = high_resolution_clock::now();
#endif

	backend.draw(width, height, dpr, go);

#ifndef NDEBUG
	auto render_stop = high_resolution_clock::now();
	auto render_duration = duration_cast<microseconds>(render_stop - render_start);
	std::clog << "Render time: " << render_duration.count() << std::endl;
#endif

#ifndef NDEBUG
	auto copy_start = high_resolution_clock::now();
#endif

	std::vector<unsigned char> out;

	unsigned char* screen = new unsigned char[width * height * 3];
	unsigned char* flip = new unsigned char[width * height * 3];

	backend.get_pixels(width, height, screen);

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

	delete[] screen;
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
