#
# Copyright (C) 2020 Giulio Girardi.
#
# This file is part of xeus-octave.
#
# xeus-octave is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# xeus-octave is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with xeus-octave.  If not, see <http://www.gnu.org/licenses/>.
#
cmake_minimum_required(VERSION 3.15)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

project(xeus-octave
        VERSION 0.0.1)

include(FetchContent)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

################################################################################
# FETCH LIBRARIES
################################################################################

FetchContent_Declare(
  cppzmq
  GIT_REPOSITORY https://github.com/zeromq/cppzmq.git
  GIT_TAG        v4.7.1
)

FetchContent_Declare(
  xtl
  GIT_REPOSITORY https://github.com/xtensor-stack/xtl.git
  GIT_TAG        0.7.2
)

FetchContent_Declare(
  xeus
  GIT_REPOSITORY https://github.com/jupyter-xeus/xeus.git
  GIT_TAG        1.0.0
)

FetchContent_Declare(
  xproperty
  GIT_REPOSITORY https://github.com/jupyter-xeus/xproperty.git
  GIT_TAG        0.11.0
)

FetchContent_Declare(
  xwidgets
  GIT_REPOSITORY https://github.com/jupyter-xeus/xwidgets.git
  GIT_TAG        f21198f5934c90b034afee9b36c47b0e7b456c6b
)

FetchContent_Declare(
  lodepng
  GIT_REPOSITORY https://github.com/lvandeve/lodepng.git
  GIT_TAG        7fdcc96a5e5864eee72911c3ca79b1d9f0d12292
)

macro(FetchContent_MakeAvailableCustom content)
  FetchContent_GetProperties(${content})
  if(NOT ${content}_POPULATED)
    FetchContent_Populate(${content})
    add_subdirectory(${${content}_SOURCE_DIR} ${${content}_BINARY_DIR} EXCLUDE_FROM_ALL)
    # This is a hack...
    set(${content}_DIR ${${content}_BINARY_DIR})
  endif()
endmacro()

# cppzmq
set(CPPZMQ_BUILD_TESTS OFF)
FetchContent_MakeAvailableCustom(cppzmq)

# xtl
FetchContent_MakeAvailableCustom(xtl)

# xeus
set(XEUS_BUILD_SHARED_LIBS OFF)
FetchContent_MakeAvailableCustom(xeus)

# xproperty
FetchContent_MakeAvailableCustom(xproperty)

# xwidgets
set(XWIDGETS_BUILD_SHARED_LIBS OFF)
FetchContent_MakeAvailableCustom(xwidgets)

# lodepng: manual loading
FetchContent_GetProperties(lodepng)
if(NOT lodepng_POPULATED)
  FetchContent_Populate(lodepng)

  add_library(lodepng STATIC ${lodepng_SOURCE_DIR}/lodepng.cpp  ${lodepng_SOURCE_DIR}/lodepng.h)
  target_include_directories(lodepng PUBLIC $<BUILD_INTERFACE:${lodepng_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
endif()

################################################################################
# FIND OCTAVE
################################################################################
find_package(PkgConfig REQUIRED)
find_package(xeus REQUIRED)
find_package(xwidgets REQUIRED)
find_package(xproperty REQUIRED)

# X11 and OpenGL are required for fast native rendering
find_package(X11)
find_package(OpenGL)

configure_file(
  "${CMAKE_SOURCE_DIR}/config.h.in"
  "${CMAKE_BINARY_DIR}/config.h"
)

pkg_check_modules(octinterp REQUIRED IMPORTED_TARGET octinterp)

# Osmesa is required for slow CPU native rendering
pkg_check_modules(osmesa IMPORTED_TARGET osmesa)

set(EXECUTABLE_NAME xeus-octave)

# We generate the kernel.json file, given the installation prefix and the executable name
configure_file(
    "${CMAKE_SOURCE_DIR}/share/jupyter/kernels/xeus-octave/kernel.json.in"
    "${CMAKE_BINARY_DIR}/kernel.json"
)

# my_kernel source files
set(XEUS_OCTAVE_SRC
    src/xoctave_interpreter.cpp
    src/xoctave_interpreter.hpp
    src/input.cpp
    src/input.hpp
    src/toolkits/plotly.cpp
    src/toolkits/plotly.hpp
    src/toolkits/notebook.cpp
    src/toolkits/notebook.hpp
    src/toolkits/tex2html.hpp
    src/xoctave/display.cpp
    src/xoctave/display.hpp
    src/main.cpp
)

include_directories(
  src
  "${CMAKE_BINARY_DIR}"
)

add_executable(${EXECUTABLE_NAME} ${XEUS_OCTAVE_SRC})
# Override path needed by kernel
target_compile_definitions(${EXECUTABLE_NAME} PRIVATE XOCTAVE_OVERRIDE_PATH="${CMAKE_INSTALL_PREFIX}/share/xeus-octave")

target_link_libraries(${EXECUTABLE_NAME} xeus-static xwidgets-static xproperty pthread)
target_link_libraries(${EXECUTABLE_NAME} PkgConfig::octinterp)
target_link_libraries(${EXECUTABLE_NAME} lodepng)

if(X11_FOUND AND OPENGL_FOUND)
  # Use hardware rendering
  message(STATUS "Notebook graphics toolkit will use hardware rendering")
  target_link_libraries(${EXECUTABLE_NAME} X11::X11 OpenGL::GL)
elseif()
  # Use software rendering
  message(STATUS "Notebook graphics toolkit will use software rendering")
  target_link_libraries(${EXECUTABLE_NAME} PkgConfig::osmesa)
else()
  message(STATUS "No X11/OpenGL or osmesa found, the notebook graphics toolkit will be disabled")
endif()

if (APPLE)
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        MACOSX_RPATH ON
    )
else()
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH 1
        SKIP_BUILD_RPATH FALSE
    )
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

# Installation
# ============

# Install my_kernel
install(TARGETS ${EXECUTABLE_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/share/jupyter/kernels/xeus-octave
        DESTINATION "share/jupyter/kernels"
        PATTERN "*.in" EXCLUDE)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/share/xeus-octave
        DESTINATION "share"
        PATTERN "*.in" EXCLUDE)

install(FILES ${CMAKE_BINARY_DIR}/kernel.json
        DESTINATION "share/jupyter/kernels/xeus-octave")