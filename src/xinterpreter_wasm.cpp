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

xoctave_wasm_interpreter* xoctave_wasm_interpreter::s_wasm_instance = nullptr;

xoctave_wasm_interpreter::xoctave_wasm_interpreter()
{
  s_wasm_instance = this;
  m_octave_interpreter.initialize_load_path(false);
  m_octave_interpreter.initialize();
}

xoctave_wasm_interpreter& xoctave_wasm_interpreter::get_instance()
{
  if (!s_wasm_instance)
  {
    throw std::runtime_error("xoctave_interpreter instance not initialized");
  }
  return *s_wasm_instance;
}

}  // namespace xeus_octave
