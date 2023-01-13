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

#ifndef XEUS_OCTAVE_NOTEBOOK_TOOLKIT_H
#define XEUS_OCTAVE_NOTEBOOK_TOOLKIT_H

#include <vector>

#include <octave/graphics-toolkit.h>
#include <octave/interpreter.h>

#include "xeus-octave/config.hpp"

namespace xeus_octave::tk::notebook
{

/**
 * Generic graphics_toolkit for rendering within a GLFW context. It cannot be
 * used on its own, but must be inherited from the actual toolkits
 */
class glfw_graphics_toolkit : public octave::base_graphics_toolkit
{
public:

  glfw_graphics_toolkit(std::string const&);
  ~glfw_graphics_toolkit();

  bool initialize(octave::graphics_object const&) override;
  void redraw_figure(octave::graphics_object const&) const override;
  virtual void send_figure(octave::graphics_object const&, std::vector<char> const&, int, int, double) const = 0;
};

/**
 * Only "user ready" toolkit. It is based on simple display_data calls. Shows
 * an empty image on creation with a display_data call. Subsequent redraws are
 * served by a update_display_data call.
 */
class notebook_graphics_toolkit : public glfw_graphics_toolkit
{
public:

  notebook_graphics_toolkit() : glfw_graphics_toolkit("notebook") {}

  bool is_valid() const override { return true; }

  bool initialize(octave::graphics_object const&) override;
  void send_figure(octave::graphics_object const&, std::vector<char> const&, int, int, double) const override;
  void show_figure(octave::graphics_object const&) const override;
};

void register_all(octave::interpreter& interpreter);

}  // namespace xeus_octave::tk::notebook

#endif  // XEUS_OCTAVE_NOTEBOOK_TOOLKIT_H
