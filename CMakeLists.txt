# ##################################################################################################
# Copyright (c) 2022, Giulio Girardi
#
# Distributed under the terms of the GNU General Public License v3.
#
# The full license is in the file LICENSE, distributed with this software.
# ##################################################################################################

cmake_minimum_required(VERSION 3.24)
project(xeus-octave)

set(XEUS_OCTAVE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Versionning
# ===========

file(
    STRINGS "${XEUS_OCTAVE_INCLUDE_DIR}/xeus-octave/config.hpp" version_defines
    REGEX "#define XEUS_OCTAVE_VERSION_(MAJOR|MINOR|PATCH)"
)
foreach(ver ${version_defines})
    if(ver MATCHES "#define XEUS_OCTAVE_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(
            XEUS_OCTAVE_VERSION_${CMAKE_MATCH_1}
            "${CMAKE_MATCH_2}"
            CACHE INTERNAL ""
        )
    endif()
endforeach()
set(
    ${PROJECT_NAME}_VERSION
    ${XEUS_OCTAVE_VERSION_MAJOR}.${XEUS_OCTAVE_VERSION_MINOR}.${XEUS_OCTAVE_VERSION_PATCH}
)
message(STATUS "Building xeus-octave v${${PROJECT_NAME}_VERSION}")

# Configuration
# =============

include(GNUInstallDirs)

if(NOT DEFINED XEUS_OCTAVE_KERNELSPEC_PATH)
    set(XEUS_OCTAVE_KERNELSPEC_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/")
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xoctave/kernel.json.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xoctave/kernel.json"
)

# Build options
# =============

option(XEUS_OCTAVE_DISABLE_ARCH_NATIVE "disable -march=native flag" OFF)
option(XEUS_OCTAVE_DISABLE_TUNE_GENERIC "disable -mtune=generic flag" OFF)

option(XEUS_OCTAVE_BUILD_STATIC "Build xeus-octave static library" OFF)
option(XEUS_OCTAVE_BUILD_SHARED "Split xoctave build into executable and library" ON)
option(XEUS_OCTAVE_BUILD_EXECUTABLE "Build the xoctave executable" ON)

option(
    XEUS_OCTAVE_USE_SHARED_XEUS
    "Link xeus-octave  with the xeus shared library (instead of the static library)"
    ON
)
option(
    XEUS_OCTAVE_USE_SHARED_XEUS_OCTAVE
    "Link xoctave  with the xeus shared library (instead of the static library)"
    ON
)

# Dependencies
# ============

set(xeus_zmq_REQUIRED_VERSION 3.0.0)

find_package(xeus-zmq REQUIRED)
find_package(PNG REQUIRED)
find_package(glad REQUIRED)
find_package(glfw3)

find_package(PkgConfig REQUIRED)
pkg_check_modules(octinterp REQUIRED IMPORTED_TARGET GLOBAL octinterp)

# Flags =====
include(CheckCXXCompilerFlag)

if(
    CMAKE_CXX_COMPILER_ID MATCHES "Clang"
    OR CMAKE_CXX_COMPILER_ID MATCHES "GNU"
    OR CMAKE_CXX_COMPILER_ID MATCHES "Intel"
)
    if(XEUS_OCTAVE_DISABLE_ARCH_NATIVE AND NOT XEUS_OCTAVE_DISABLE_TUNE_GENERIC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic")
    elseif(XEUS_OCTAVE_DISABLE_TUNE_GENERIC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif()
endif()

# Source files
# ============

set(
    XEUS_OCTAVE_HEADERS
    include/xeus-octave/config.hpp
    include/xeus-octave/xinterpreter.hpp
    include/xeus-octave/input.hpp
    include/xeus-octave/output.hpp
    include/xeus-octave/utils.hpp
    include/xeus-octave/tk_notebook.hpp
    include/xeus-octave/opengl.hpp
    include/xeus-octave/tk_plotly.hpp
    include/xeus-octave/plotstream.hpp
    include/xeus-octave/tex2html.hpp
    include/xeus-octave/display.hpp
)

set(
    XEUS_OCTAVE_SRC
    src/tk_plotly.cpp
    src/tk_notebook.cpp
    src/display.cpp
    src/xinterpreter.cpp
    src/input.cpp
    src/output.cpp
)

set(XEUS_OCTAVE_MAIN_SRC src/main.cpp)

# Targets and link - Macros
# =========================

include(CheckCXXCompilerFlag)
include(cmake/CompilerWarnings.cmake)

set(
    CMAKE_INSTALL_RPATH
    "${CMAKE_INSTALL_PREFIX}/lib; ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
)

macro(xeus_octave_set_common_options target_name)
    xeus_octave_target_add_compile_warnings(${target_name})

    target_compile_features(${target_name} PRIVATE cxx_std_17)
    if(
        CMAKE_CXX_COMPILER_ID MATCHES "Clang"
        OR CMAKE_CXX_COMPILER_ID MATCHES "GNU"
        OR CMAKE_CXX_COMPILER_ID MATCHES "Intel"
    )
        # Mtune generic/native
        if(XEUS_OCTAVE_DISABLE_ARCH_NATIVE AND NOT XEUS_OCTAVE_DISABLE_TUNE_GENERIC)
            target_compile_options(${target_name} PUBLIC -mtune=generic)
        elseif(XEUS_OCTAVE_DISABLE_TUNE_GENERIC)

        else()
            target_compile_options(${target_name} PUBLIC -march=native)
        endif()

    endif()

    if(APPLE)
        set_target_properties(${target_name} PROPERTIES MACOSX_RPATH ON)
    else()
        set_target_properties(
            ${target_name} PROPERTIES BUILD_WITH_INSTALL_RPATH 1 SKIP_BUILD_RPATH FALSE
        )
    endif()

    set_target_properties(${target_name} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
endmacro()

# Common macro kernels (xoctave )
macro(xeus_octave_set_kernel_options target_name)
    if(XEUS_OCTAVE_USE_SHARED_XEUS_OCTAVE)
        target_link_libraries(${target_name} PRIVATE xeus-octave)
        if(CMAKE_DL_LIBS)
            target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS} util)
        endif()
    else()
        target_link_libraries(${target_name} PRIVATE xeus-octave-static)
    endif()

    find_package(Threads)
    target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})

endmacro()

# Scripts directory for xeus-octave
set(
    XEUS_OCTAVE_SCRIPTS_BASEDIR
    "share"
    CACHE STRING "Xeus-octave scripts base directory"
)

# Common macro for shared and static library
macro(xeus_octave_create_target target_name linkage output_name)
    string(TOUPPER "${linkage}" linkage_upper)

    if(NOT ${linkage_upper} MATCHES "^(SHARED|STATIC)$")
        message(FATAL_ERROR "Invalid library linkage: ${linkage}")
    endif()

    add_library(${target_name} ${linkage_upper} ${XEUS_OCTAVE_SRC} ${XEUS_OCTAVE_HEADERS})
    xeus_octave_set_common_options(${target_name})

    set_target_properties(
        ${target_name}
        PROPERTIES
            PUBLIC_HEADER "${XEUS_OCTAVE_HEADERS}"
            PREFIX ""
            VERSION ${${PROJECT_NAME}_VERSION}
            SOVERSION ${XEUS_OCTAVE_VERSION_MAJOR}
            OUTPUT_NAME "lib${output_name}"
    )

    target_compile_definitions(
        ${target_name}
        PUBLIC "XEUS_OCTAVE_EXPORTS"
        PRIVATE XEUS_OCTAVE_OVERRIDE_PATH="${CMAKE_INSTALL_PREFIX}/share/xeus-octave"
    )

    target_include_directories(
        ${target_name}
        PUBLIC $<BUILD_INTERFACE:${XEUS_OCTAVE_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>
    )

    target_link_libraries(
        ${target_name}
        PUBLIC PkgConfig::octinterp
        PRIVATE glad::glad glfw PNG::PNG
    )
    if(XEUS_OCTAVE_USE_SHARED_XEUS)
        target_link_libraries(${target_name} PUBLIC xeus-zmq)
    else()
        target_link_libraries(${target_name} PUBLIC xeus-zmq-static)
    endif()

    if(WIN32 OR CYGWIN)
        #
    elseif(APPLE)
        target_link_libraries(${target_name} PRIVATE "-undefined dynamic_lookup")
    endif()
    find_package(Threads)
    target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})

endmacro()

# xeus-octave
# ===========

set(XEUS_OCTAVE_TARGETS "")

if(XEUS_OCTAVE_BUILD_SHARED)
    # Build libraries
    xeus_octave_create_target(xeus-octave SHARED xeus-octave)
    list(APPEND XEUS_OCTAVE_TARGETS xeus-octave)
endif()

if(XEUS_OCTAVE_BUILD_STATIC)
    # On Windows, a static library should use a different output name to avoid the conflict with the
    # import library of a shared one.
    if(CMAKE_HOST_WIN32)
        xeus_octave_create_target(xeus-octave-static STATIC xeus-octave-static)
    else()
        xeus_octave_create_target(xeus-octave-static STATIC xeus-octave)
    endif()
    list(APPEND XEUS_OCTAVE_TARGETS xeus-octave-static)
endif()

# xeus-octave
# =======
if(XEUS_OCTAVE_BUILD_EXECUTABLE)
    add_executable(xoctave ${XEUS_OCTAVE_MAIN_SRC})
    xeus_octave_set_common_options(xoctave)
    xeus_octave_set_kernel_options(xoctave)
endif()

# Installation
# ============

include(CMakePackageConfigHelpers)

set(
    XEUS_OCTAVE_CMAKECONFIG_INSTALL_DIR
    "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    CACHE STRING "install path for xeus-octaveConfig.cmake"
)

# Install xeus-octave and xeus-octave-static
if(XEUS_OCTAVE_BUILD_SHARED)
    install(
        TARGETS ${XEUS_OCTAVE_TARGETS}
        EXPORT ${PROJECT_NAME}-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xeus-octave
    )

    # Makes the project importable from the build directory
    export(
        EXPORT ${PROJECT_NAME}-targets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
    )
endif()

# Install xoctave
if(XEUS_OCTAVE_BUILD_EXECUTABLE)
    install(TARGETS xoctave RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    # Configuration and data directories for jupyter and xeus-octave
    set(
        XJUPYTER_DATA_DIR
        "share/jupyter"
        CACHE STRING "Jupyter data directory"
    )

    # Install xoctave Jupyter kernelspec
    set(KERNELSPEC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels)
    install(
        DIRECTORY ${KERNELSPEC_DIR}
        DESTINATION ${XJUPYTER_DATA_DIR}
        PATTERN "*.in" EXCLUDE
    )

    # Install xoctave scripts
    set(XOCTAVE_SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share/xeus-octave)
    install(
        DIRECTORY ${XOCTAVE_SCRIPTS_DIR}
        DESTINATION ${XEUS_OCTAVE_SCRIPTS_BASEDIR}
        PATTERN "*.in" EXCLUDE
    )

    # Extra path for installing Jupyter kernelspec
    if(XEXTRA_JUPYTER_DATA_DIR)
        install(
            DIRECTORY ${KERNELSPEC_DIR}
            DESTINATION ${XEXTRA_JUPYTER_DATA_DIR}
            PATTERN "*.in" EXCLUDE
        )
    endif()
endif()

# Configure 'xeus-octaveConfig.cmake' for a build tree
set(XEUS_OCTAVE_CONFIG_CODE "####### Expanded from \@XEUS_OCTAVE_CONFIG_CODE\@ #######\n")
set(
    XEUS_OCTAVE_CONFIG_CODE
    "${XEUS_OCTAVE_CONFIG_CODE}set(CMAKE_MODULE_PATH \"${CMAKE_CURRENT_SOURCE_DIR}/cmake;\${CMAKE_MODULE_PATH}\")\n"
)
set(
    XEUS_OCTAVE_CONFIG_CODE
    "${XEUS_OCTAVE_CONFIG_CODE}##################################################"
)
configure_package_config_file(
    ${PROJECT_NAME}Config.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${PROJECT_BINARY_DIR}
)

# Configure 'xeus-octaveConfig.cmake.in for an install tree
set(XEUS_OCTAVE_CONFIG_CODE "")
configure_package_config_file(
    ${PROJECT_NAME}Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${XEUS_OCTAVE_CMAKECONFIG_INSTALL_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${${PROJECT_NAME}_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake
          ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${XEUS_OCTAVE_CMAKECONFIG_INSTALL_DIR}
)

if(XEUS_OCTAVE_BUILD_SHARED)
    install(
        EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}Targets.cmake
        DESTINATION ${XEUS_OCTAVE_CMAKECONFIG_INSTALL_DIR}
    )
endif()
