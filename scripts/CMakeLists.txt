# set the list of documents to process
execute_process(
    COMMAND
        python -c
        "from ipywidgets.widgets import Widget; print(';'.join([d[2].removesuffix('Model') + '.m' for d, _ in Widget._widget_types.items()]), end='')"
    OUTPUT_VARIABLE WIDGETS
)

set(WIDGETS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
# set(WIDGETS_OUT_DIR ${CMAKE_BINARY_DIR}/share/xeus-octave/+xwidgets)

# add the custom commands for each document
foreach(WIDGET ${WIDGETS})
    if(EXISTS "${WIDGETS_DIR}/${WIDGET}")
        add_custom_command(
            OUTPUT ${WIDGET}
            DEPENDS ${WIDGETS_DIR}/${WIDGET}
            COMMAND cog -o ${WIDGET} -d ${WIDGETS_DIR}/${WIDGET}
            VERBATIM
        )
    else()
        add_custom_command(
            OUTPUT ${WIDGET}
            DEPENDS ${WIDGETS_DIR}/Generic.m
            COMMAND cog -o ${WIDGET} -d ${WIDGETS_DIR}/Generic.m
            VERBATIM
        )
    endif()

    # build a list of all the results
    list(PREPEND WIDGET_RESULTS ${WIDGET})
endforeach()

# finally add the custom target that when invoked will cause the generation of the pdf file
add_custom_target(widgets ALL DEPENDS ${WIDGET_RESULTS})
