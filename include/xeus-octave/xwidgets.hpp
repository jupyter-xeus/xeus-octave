/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_XWIDGETS_HPP
#define XEUS_OCTAVE_XWIDGETS_HPP

#include <octave/interpreter.h>

namespace xeus_octave::widgets {
void register_all(octave::interpreter &interpreter);
}

#endif