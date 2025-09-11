/***************************************************************************
 * Copyright (c) 2025, QuantStack                                           *
 *                                                                          *
 * Distributed under the terms of the GNU General Public License v3.        *
 *                                                                          *
 * The full license is in the file LICENSE, distributed with this software. *
 ****************************************************************************/

#ifndef XEUS_OCTAVE_INTERPRETER_WASM_HPP
#define XEUS_OCTAVE_INTERPRETER_WASM_HPP

#include "xeus-octave/xinterpreter.hpp"

namespace xeus_octave
{

class XEUS_OCTAVE_API xoctave_wasm_interpreter : public xoctave_interpreter
{

public:

  xoctave_wasm_interpreter();
  virtual ~xoctave_wasm_interpreter() = default;
};

}  // namespace xeus_octave

#endif  // XEUS_OCTAVE_INTERPRETER_WASM_HPP
