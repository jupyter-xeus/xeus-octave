# ##################################################################################################
# Copyright (c) 2023, Giulio Girardi
#
# Distributed under the terms of the GNU General Public License v3.
#
# The full license is in the file LICENSE, distributed with this software.
# ##################################################################################################

execute_process(
    COMMAND
        python -c
        "from ipywidgets.widgets import Widget; print(';'.join([d[2].removesuffix('Model') + '.m' for d, _ in Widget._widget_types.items()]), end='')"
    OUTPUT_VARIABLE XEUS_OCTAVE_WIDGETS
)

set(XEUS_OCTAVE_WIDGETS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

foreach(WIDGET ${XEUS_OCTAVE_WIDGETS})
    if(EXISTS ${XEUS_OCTAVE_WIDGETS_SOURCE_DIR}/${WIDGET})
        add_custom_command(
            OUTPUT ${WIDGET}
            DEPENDS ${XEUS_OCTAVE_WIDGETS_SOURCE_DIR}/${WIDGET}
            COMMAND cog -o ${WIDGET} -d ${XEUS_OCTAVE_WIDGETS_SOURCE_DIR}/${WIDGET}
            VERBATIM
        )
    else()
        add_custom_command(
            OUTPUT ${WIDGET}
            DEPENDS ${XEUS_OCTAVE_WIDGETS_SOURCE_DIR}/Generic.m
            COMMAND cog -o ${WIDGET} -d ${XEUS_OCTAVE_WIDGETS_SOURCE_DIR}/Generic.m
            VERBATIM
        )
    endif()

    list(PREPEND XEUS_OCTAVE_GENERATED_WIDGETS ${CMAKE_CURRENT_BINARY_DIR}/${WIDGET})
endforeach()

install(
    FILES ${XEUS_OCTAVE_GENERATED_WIDGETS}
    DESTINATION ${XEUS_OCTAVE_SCRIPTS_BASEDIR}/xeus-octave/+xwidgets
)

add_custom_target(widgets ALL DEPENDS ${XEUS_OCTAVE_WIDGETS})
