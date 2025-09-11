/***************************************************************************
 * Copyright (c) 2025, QuantStack                                           *
 *                                                                          *
 * Distributed under the terms of the GNU General Public License v3.        *
 *                                                                          *
 * The full license is in the file LICENSE, distributed with this software. *
 ****************************************************************************/

#include "xeus-octave/xinterpreter_wasm.hpp"

namespace xeus_octave
{

xoctave_wasm_interpreter::xoctave_wasm_interpreter()
{
  m_octave_interpreter.initialize_load_path(false);
  m_octave_interpreter.initialize();
}

}  // namespace xeus_octave
