/***************************************************************************
* Copyright (c) 2022, Giulio Girardi
*
* Distributed under the terms of the GNU General Public License v3.
*
* The full license is in the file LICENSE, distributed with this software.
****************************************************************************/


#ifndef XEUS_OCTAVE_BINDINGS_HPP
#define XEUS_OCTAVE_BINDINGS_HPP

#define XEUS_OCTAVE_LANGUAGE_BINDING(interpreter, method, function) \
    auto __##function##_func = new octave_builtin(function, method, __FILE__, ""); \
    interpreter.get_symbol_table().install_built_in_function(method, __##function##_func);

#endif