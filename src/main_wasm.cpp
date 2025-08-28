/***************************************************************************
* Copyright (c) 2025, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the GNU General Public License v3.        *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/


#include <emscripten/bind.h>

#include <xeus/xembind.hpp>

#include "xeus-octave/xinterpreter_wasm.hpp"


EMSCRIPTEN_BINDINGS(my_module)
{
    xeus::export_core();
    using interpreter_type = xeus_octave::wasm_interpreter;
    xeus::export_kernel<interpreter_type>("xkernel");
}