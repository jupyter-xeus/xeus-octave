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
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(xeus-octave
        VERSION 0.0.1)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GLFW3_OSMESA_BACKEND FALSE CACHE BOOL "Use the osmesa GLFW backend (for software rendering)")

# Fetch libraries
################################################################################

include(FindOrFetch)

find_or_fetch_package(
  cppzmq
  https://github.com/zeromq/cppzmq.git v4.7.1
  CPPZMQ_BUILD_TESTS OFF
)

find_or_fetch_package(
  xtl
  https://github.com/xtensor-stack/xtl.git 0.7.2
)

find_or_fetch_package(
  xeus
  https://github.com/jupyter-xeus/xeus.git 1.0.0
  XEUS_BUILD_SHARED_LIBS OFF
)

find_or_fetch_package(
  glfw3
  https://github.com/glfw/glfw 3.3.4
  GLFW_USE_OSMESA ${GLFW3_OSMESA_BACKEND}
  BUILD_SHARED_LIBS OFF
)

# Deps
################################################################################
find_package(PNG REQUIRED)

# Octave
################################################################################
find_package(PkgConfig REQUIRED)

pkg_check_modules(octinterp REQUIRED IMPORTED_TARGET GLOBAL octinterp)

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
    src/io.cpp
    src/io.hpp
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

if (XEUS_FETCH)
  target_link_libraries(${EXECUTABLE_NAME} xeus-static)
else()
  target_link_libraries(${EXECUTABLE_NAME} xeus)
endif()

target_link_libraries(${EXECUTABLE_NAME} pthread)
target_link_libraries(${EXECUTABLE_NAME} PkgConfig::octinterp)

if (GLFW3_OSMESA_BACKEND)
  pkg_check_modules(osmesa REQUIRED IMPORTED_TARGET osmesa)
  target_link_libraries(${EXECUTABLE_NAME} PkgConfig::osmesa)
  set(NOTEBOOK_TOOLKIT_CPU TRUE)
else()
  find_package(OpenGL REQUIRED)
  target_link_libraries(${EXECUTABLE_NAME} OpenGL::GL)
endif()

target_link_libraries(${EXECUTABLE_NAME} glfw)
target_link_libraries(${EXECUTABLE_NAME} PNG::PNG)

set(NOTEBOOK_TOOLKIT_ENABLED ON)

configure_file(
  "${CMAKE_SOURCE_DIR}/config.h.in"
  "${CMAKE_BINARY_DIR}/config.h"
)

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
################################################################################

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