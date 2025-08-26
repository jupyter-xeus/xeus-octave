/***************************************************************************
* Copyright (c) 2025, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the GNU General Public License v3.        *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/


#include "xeus/xinterpreter.hpp"
#include "xeus/xsystem.hpp"

#include "xeus-octave/xinterpreter.hpp"
#include "xeus-octave/xinterpreter_wasm.hpp"

namespace xeus_octave
{
    wasm_interpreter::wasm_interpreter()
        : xoctave_interpreter()
    {
    }
}