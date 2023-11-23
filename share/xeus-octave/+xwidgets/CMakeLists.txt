# ##################################################################################################
# Copyright (c) 2023, Giulio Girardi
#
# Distributed under the terms of the GNU General Public License v3.
#
# The full license is in the file LICENSE, distributed with this software.
# ##################################################################################################

execute_process(
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/xwidgets_generate.py
    OUTPUT_VARIABLE XEUS_OCTAVE_WIDGETS
)

set(XEUS_OCTAVE_WIDGETS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(
    OUTPUT ${XEUS_OCTAVE_WIDGETS}
    DEPENDS ${XEUS_OCTAVE_WIDGETS_SOURCE_DIR}/Widget.m
    COMMAND
        python ${CMAKE_CURRENT_SOURCE_DIR}/xwidgets_generate.py
        ${XEUS_OCTAVE_WIDGETS_SOURCE_DIR}/Widget.m
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
)

list(
    TRANSFORM XEUS_OCTAVE_WIDGETS
    PREPEND "${CMAKE_CURRENT_BINARY_DIR}/" OUTPUT_VARIABLE XEUS_OCTAVE_GENERATED_WIDGETS
)

install(
    FILES ${XEUS_OCTAVE_GENERATED_WIDGETS}
    DESTINATION ${XEUS_OCTAVE_SCRIPTS_BASEDIR}/xeus-octave/+xwidgets
)

add_custom_target(widgets ALL DEPENDS ${XEUS_OCTAVE_WIDGETS})
