/*
 * Copyright (C) 2022 Giulio Girardi.
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

#include "xeus-octave/input.hpp"

#include <octave/cmd-edit.h>

#include "xeus-octave/xinterpreter.hpp"

namespace
{

static octave::command_editor* old;

/**
 * Crazy hack to access the private member
 */
template <octave::command_editor** M> struct AccessInstance
{
  friend void _set_command_editor(octave::command_editor& n)
  {
    // Backup old instance
    old = *M;

    // Override s_instance
    *M = &n;
  }

  friend void _reset_command_editor() { *M = old; }
};

template struct AccessInstance<&octave::command_editor::s_instance>;

void _set_command_editor(octave::command_editor& n);
void _reset_command_editor();

}  // namespace

namespace xeus_octave::io
{

std::string xoctave_input::do_readline(std::string const& prompt, bool&)
{
  // Interpreter reference
  xeus_octave::xoctave_interpreter& interpreter =
    dynamic_cast<xeus_octave::xoctave_interpreter&>(xeus::get_interpreter());
  // Read value
  std::string value;

  // Register a input handler (called when input is ready)
  interpreter.register_input_handler([&value](std::string const& v) { value = v; });

  // Send the input request
  interpreter.input_request(prompt, false);

  // Remove input handler
  interpreter.register_input_handler(nullptr);

  return value;
}

void register_input(xoctave_input& input)
{
  _set_command_editor(input);
}

}  // namespace xeus_octave::io
