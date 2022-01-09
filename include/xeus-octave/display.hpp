/***************************************************************************
* Copyright (c) 2022, Giulio Girardi
*
* Distributed under the terms of the GNU General Public License v3.
*
* The full license is in the file LICENSE, distributed with this software.
****************************************************************************/


#ifndef XEUS_OCTAVE_DISPLAY_HPP
#define XEUS_OCTAVE_DISPLAY_HPP

#include <octave/interpreter.h>

namespace xeus_octave::display {
	void register_all(octave::interpreter &interpreter);
}

#endif