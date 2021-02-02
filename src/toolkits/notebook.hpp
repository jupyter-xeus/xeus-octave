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

#ifndef NOTEBOOK_TOOLKIT_H
#define NOTEBOOK_TOOLKIT_H

#include "config.h"

#ifdef NOTEBOOK_TOOLKIT_ENABLED

#include <GLFW/glfw3.h>
#include <octave/graphics-toolkit.h>
#include <octave/interpreter.h>

#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>

#include "opengl.hpp"
#include "xwidgets/xtransport.hpp"
#include "xwidgets/xwidget.hpp"

namespace xoctave {

class notebook_graphics_toolkit : public octave::base_graphics_toolkit {
public:
	notebook_graphics_toolkit(octave::interpreter &);
	~notebook_graphics_toolkit();

	bool is_valid() const override { return true; }

	bool initialize(const graphics_object &) override;
	void redraw_figure(const graphics_object &) const override;
	void show_figure(const graphics_object &) const override;
	void update(const graphics_object &, int) override;

	void finalize(const graphics_object &) override;

private:
	GLFWwindow *window = nullptr;
	octave::interpreter &m_interpreter;
};

}  // namespace xoctave

#endif

#endif