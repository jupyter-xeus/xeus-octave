/***************************************************************************
* Copyright (c) 2025, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the GNU General Public License v3.        *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/


#ifndef XEUS_OCTAVE_INTERPRETER_WASM_HPP
#define XEUS_OCTAVE_INTERPRETER_WASM_HPP


#include "xinterpreter.hpp"
#include "config.hpp"

namespace xeus_octave
{
    class XEUS_OCTAVE_API wasm_interpreter : public xoctave_interpreter
    {

    public:
        wasm_interpreter();
        virtual ~wasm_interpreter() = default;

    };

}

#endif  // XEUS_OCTAVE_INTERPRETER_WASM_HPP