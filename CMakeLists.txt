#############################################################################
#Copyright (c) 2022,
#
#Distributed under the terms of the GNU General Public License v3.
#
#The full license is in the file LICENSE, distributed with this software.
#############################################################################

cmake_minimum_required(VERSION 3.4.3)
project(xeus-octave)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

set(XEUS_OCTAVE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Versionning
# ===========

file(STRINGS "${XEUS_OCTAVE_INCLUDE_DIR}/xeus-octave/xeus_octave_config.hpp" version_defines
     REGEX "#define XEUS_OCTAVE_VERSION_(MAJOR|MINOR|PATCH)")
foreach (ver ${version_defines})
    if (ver MATCHES "#define XEUS_OCTAVE_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(XEUS_OCTAVE_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif ()
endforeach ()
set(${PROJECT_NAME}_VERSION
    ${XEUS_OCTAVE_VERSION_MAJOR}.${XEUS_OCTAVE_VERSION_MINOR}.${XEUS_OCTAVE_VERSION_PATCH})
message(STATUS "Building xeus-octave v${${PROJECT_NAME}_VERSION}")

# Configuration
# =============

include(GNUInstallDirs)

if (NOT DEFINED XEUS_OCTAVE_KERNELSPEC_PATH)
    set(XEUS_OCTAVE_KERNELSPEC_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/")
endif ()



configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xoctave/kernel.json.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xoctave/kernel.json"
)

# Build options
# =============

# Compilation options
OPTION(XEUS_OCTAVE_DISABLE_ARCH_NATIVE "disable -march=native flag" OFF)
OPTION(XEUS_OCTAVE_DISABLE_TUNE_GENERIC "disable -mtune=generic flag" OFF)

OPTION(XEUS_OCTAVE_BUILD_STATIC "Build xeus-octave static library" ON)
OPTION(XEUS_OCTAVE_BUILD_SHARED "Split xoctave build into executable and library" ON)
OPTION(XEUS_OCTAVE_BUILD_EXECUTABLE "Build the xoctave executable" ON)

OPTION(XEUS_OCTAVE_USE_SHARED_XEUS "Link xoctave  with the xeus shared library (instead of the static library)" ON)
OPTION(XEUS_OCTAVE_USE_SHARED_XEUS_OCTAVE "Link xoctave  with the xeus shared library (instead of the static library)" ON)
OPTION(XEUS_OCTAVE_EMSCRIPTEN_WASM_BUILD "Build for wasm with emscripten" OFF)


if(XEUS_OCTAVE_EMSCRIPTEN_WASM_BUILD)
    add_compile_definitions(XEUS_OCTAVE_EMSCRIPTEN_WASM_BUILD)
    message("Build with emscripten")
    SET(XEUS_OCTAVE_DISABLE_ARCH_NATIVE ON)
    SET(XEUS_OCTAVE_DISABLE_TUNE_GENERIC ON)
    SET(XEUS_OCTAVE_BUILD_STATIC ON)
    SET(XEUS_OCTAVE_BUILD_SHARED OFF)
    SET(XEUS_OCTAVE_BUILD_EXECUTABLE OFF)
    SET(XEUS_OCTAVE_USE_SHARED_XEUS OFF)
    SET(XEUS_OCTAVE_USE_SHARED_XEUS_OCTAVE OFF)
    SET(XEUS_OCTAVE_BUILD_TESTS OFF)
endif()


# Dependencies
# ============

set(xtl_REQUIRED_VERSION 0.6.23)
set(xeus_REQUIRED_VERSION 2.0.0)

if (NOT TARGET xtl)
    find_package(xtl ${xtl_REQUIRED_VERSION} REQUIRED)
endif ()
if (NOT TARGET xeus AND NOT TARGET xeus-static)
    find_package(xeus ${xeus_REQUIRED_VERSION} REQUIRED)
endif ()

find_package(PNG REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(glfw3 REQUIRED)
# include(FetchContent)
# FetchContent_Declare(
#   glfw3
#   GIT_REPOSITORY https://github.com/glfw/glfw
#   GIT_TAG df8d7bc892937a8b0f7c604c92a9f64f383cf48c
# )

# FetchContent_GetProperties(glfw3)
# if(NOT glfw3_POPULATED)
#   # Fetch the content using previously declared details
#   FetchContent_Populate(glfw3)

#   set(GLFW_LIBRARY_TYPE STATIC)
#   set(GLFW_INSTALL OFF)

#   # Bring the populated content into the build
#   add_subdirectory(${glfw3_SOURCE_DIR} ${glfw3_BINARY_DIR})
# endif()

pkg_check_modules(octinterp REQUIRED IMPORTED_TARGET GLOBAL octinterp)


# Flags
# =====
include(CheckCXXCompilerFlag)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4141")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4018 /wd4267 /wd4715 /wd4146 /wd4129")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    if(NOT XEUS_OCTAVE_EMSCRIPTEN_WASM_BUILD)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter -Wextra -Wreorder")
    endif()
    if (XEUS_OCTAVE_DISABLE_ARCH_NATIVE AND NOT XEUS_OCTAVE_DISABLE_TUNE_GENERIC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic")
    elseif (XEUS_OCTAVE_DISABLE_TUNE_GENERIC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif ()


    CHECK_CXX_COMPILER_FLAG("-std=c++17" HAS_CPP_17_FLAG)
    if (HAS_CPP_17_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    else ()
        message(FATAL_ERROR "Unsupported compiler -- xeus requires C++17 support!")
    endif ()

endif ()


if(XEUS_OCTAVE_EMSCRIPTEN_WASM_BUILD)

    include(XeusEmscriptenWasmFlags)

    # async
    set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} --bind")
    set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} -s ASYNCIFY=1")
    #set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} -s 'ASYNCIFY_IMPORTS=[\"async_get_input_function\"]'")
    set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} -s 'ASYNCIFY_STACK_SIZE=20480'")

    # ENV (https://github.com/emscripten-core/emscripten/commit/6d9681ad04f60b41ef6345ab06c29bbc9eeb84e0)
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s \"EXTRA_EXPORTED_RUNTIME_METHODS=[ENV']\"")

    # filesystem
    #file(COPY asset_dir DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    #set(XEUS_EMSCRIPTEN_WASM_FLAGS "${XEUS_EMSCRIPTEN_WASM_FLAGS} --embed-file asset_dir")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${XEUS_EMSCRIPTEN_WASM_FLAGS}")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS}  ${XEUS_EMSCRIPTEN_WASM_FLAGS} ---proxy-to-worker")
endif()


# Source files
# ============

set(XEUS_OCTAVE_HEADERS
    include/xeus-octave/xeus_octave_config.hpp
    include/xeus-octave/xinterpreter.hpp
)

set(XEUS_OCTAVE_SRC
    src/xinterpreter.cpp
    src/display.cpp
    src/output.cpp
    src/tk_notebook.cpp
    glad/src/glad.c
)

set(XEUS_OCTAVE_MAIN_SRC
    src/main.cpp
)


# Targets and link - Macros
# =========================

include(CheckCXXCompilerFlag)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib; ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

macro(xeus_octave_set_common_options target_name)
    if (MSVC)
        target_compile_options(${target_name} PUBLIC /wd4251 /wd4141)
        target_compile_options(${target_name} PUBLIC /wd4018 /wd4267 /wd4715 /wd4146 /wd4129)
    endif ()

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
        CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
        CMAKE_CXX_COMPILER_ID MATCHES "Intel")

        target_compile_options(${target_name} PUBLIC -Wunused-parameter -Wextra -Wreorder)

        # Mtune generic/native
        if (XEUS_OCTAVE_DISABLE_ARCH_NATIVE AND NOT XEUS_OCTAVE_DISABLE_TUNE_GENERIC)
            target_compile_options(${target_name} PUBLIC -mtune=generic)
        elseif (XEUS_OCTAVE_DISABLE_TUNE_GENERIC)
        else ()
            target_compile_options(${target_name} PUBLIC -march=native)
        endif ()

        # C++17 flag
        CHECK_CXX_COMPILER_FLAG("-std=c++17" HAS_CPP_17_FLAG)
        if (HAS_CPP_17_FLAG)
            target_compile_features(${target_name} PRIVATE cxx_std_17)
        else ()
            message(FATAL_ERROR "Unsupported compiler -- xeus-octave requires C++17 support!")
        endif ()
    endif ()

    if (APPLE)
        set_target_properties(${target_name} PROPERTIES
            MACOSX_RPATH ON
        )
    else ()
        set_target_properties(${target_name} PROPERTIES
            BUILD_WITH_INSTALL_RPATH 1
            SKIP_BUILD_RPATH FALSE
        )
    endif ()

    set_target_properties(${target_name} PROPERTIES
        INSTALL_RPATH_USE_LINK_PATH TRUE
    )
endmacro()

# Common macro kernels (xoctave )
macro(xeus_octave_set_kernel_options target_name)
    if (XEUS_OCTAVE_USE_SHARED_XEUS_OCTAVE)
        target_link_libraries(${target_name} PRIVATE xeus-octave)
        if(CMAKE_DL_LIBS)
            target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS} util)
        endif()
    else ()
        target_link_libraries(${target_name} PRIVATE xeus-octave-static)
    endif()

    if(NOT XEUS_OCTAVE_EMSCRIPTEN_WASM_BUILD)
        find_package(Threads)
        target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT}  )
    else()
        # add something to link here
        # target_link_libraries(${target_name} PRIVATE
        #     ${SOME_LIBRARIES})
    endif()

endmacro()

# Scripts directory for xeus-octave
set(XEUS_OCTAVE_SCRIPTS_BASEDIR "share"    CACHE STRING "Xeus-octave scripts base directory")

# Common macro for shared and static library
macro(xeus_octave_create_target target_name linkage output_name)
    string(TOUPPER "${linkage}" linkage_upper)

    if (NOT ${linkage_upper} MATCHES "^(SHARED|STATIC)$")
        message(FATAL_ERROR "Invalid library linkage: ${linkage}")
    endif ()

    add_library(${target_name} ${linkage_upper} ${XEUS_OCTAVE_SRC} ${XEUS_OCTAVE_HEADERS})
    xeus_octave_set_common_options(${target_name})

    set_target_properties(${target_name} PROPERTIES
                          PUBLIC_HEADER "${XEUS_OCTAVE_HEADERS}"
                          PREFIX ""
                          VERSION ${${PROJECT_NAME}_VERSION}
                          SOVERSION ${XEUS_OCTAVE_VERSION_MAJOR}
                          OUTPUT_NAME "lib${output_name}")

    target_compile_definitions(${target_name} PUBLIC "XEUS_OCTAVE_EXPORTS")

    # Override path needed by kernel
    target_compile_definitions(${target_name} PRIVATE XOCTAVE_OVERRIDE_PATH="${CMAKE_INSTALL_PREFIX}/${XEUS_OCTAVE_SCRIPTS_BASEDIR}")

    target_compile_features(${target_name} PRIVATE cxx_std_17)

    target_include_directories(${target_name}
                               PUBLIC
                               $<BUILD_INTERFACE:${XEUS_OCTAVE_INCLUDE_DIR}>
                               $<INSTALL_INTERFACE:include>)
    target_include_directories(${target_name} PRIVATE "glad/include")

    if (XEUS_OCTAVE_USE_SHARED_XEUS)
        set(XEUS_OCTAVE_XEUS_TARGET xeus)
    else ()
        set(XEUS_OCTAVE_XEUS_TARGET xeus-static)
    endif ()

    target_link_libraries(${target_name} PUBLIC ${XEUS_OCTAVE_XEUS_TARGET} xtl PkgConfig::octinterp PNG::PNG)
    target_link_libraries(${target_name} PRIVATE glfw)

    if (WIN32 OR CYGWIN)
        #
    elseif (APPLE)
        target_link_libraries(${target_name} PRIVATE "-undefined dynamic_lookup")
    endif ()

    if(NOT XEUS_OCTAVE_EMSCRIPTEN_WASM_BUILD)
        find_package(Threads) # TODO: add Threads as a dependence of xeus-static?
        target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
    else()
        # add some library here
        #target_link_libraries(${target_name} PRIVATE  ${SOME_LIBRARIES})
    endif()
endmacro()

# xeus-octave
# ===========

set(XEUS_OCTAVE_TARGETS "")

if (XEUS_OCTAVE_BUILD_SHARED)
    # Build libraries
    xeus_octave_create_target(xeus-octave SHARED xeus-octave)
    list(APPEND XEUS_OCTAVE_TARGETS xeus-octave)
endif ()

if (XEUS_OCTAVE_BUILD_STATIC)
    # On Windows, a static library should use a different output name
    # to avoid the conflict with the import library of a shared one.
    if (CMAKE_HOST_WIN32)
        xeus_octave_create_target(xeus-octave-static STATIC xeus-octave-static)
    else ()
        xeus_octave_create_target(xeus-octave-static STATIC xeus-octave)
    endif ()
    list(APPEND XEUS_OCTAVE_TARGETS xeus-octave-static)
endif ()

# xeus-octave
# =======
if (XEUS_OCTAVE_BUILD_EXECUTABLE)
    add_executable(xoctave ${XEUS_OCTAVE_MAIN_SRC})
    target_compile_features(xoctave PRIVATE cxx_std_17)
    xeus_octave_set_common_options(xoctave)
    xeus_octave_set_kernel_options(xoctave)
endif()


if(XEUS_OCTAVE_EMSCRIPTEN_WASM_BUILD)
    add_executable(xeus_kernel src/main_emscripten_kernel.cpp )
    target_link_libraries(xeus_kernel  xeus-octave-static)
endif()


# Installation
# ============

include(CMakePackageConfigHelpers)

set(XEUS_OCTAVE_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "install path for xeus-octaveConfig.cmake")

# Install xeus-octave and xeus-octave-static
if (XEUS_OCTAVE_BUILD_SHARED)
    install(TARGETS ${XEUS_OCTAVE_TARGETS}
            EXPORT ${PROJECT_NAME}-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xeus-octave)

    # Makes the project importable from the build directory
    export(EXPORT ${PROJECT_NAME}-targets
           FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
endif ()

# Install xoctave
if (XEUS_OCTAVE_BUILD_EXECUTABLE)
    install(TARGETS xoctave
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    # Configuration and data directories for jupyter and xeus-octave
    set(XJUPYTER_DATA_DIR "share/jupyter"    CACHE STRING "Jupyter data directory")

    # Install xoctave Jupyter kernelspec
    set(KERNELSPEC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels)
    install(DIRECTORY ${KERNELSPEC_DIR}
            DESTINATION ${XJUPYTER_DATA_DIR}
            PATTERN "*.in" EXCLUDE)

    # Install xoctave scripts
    set(XOCTAVE_SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share/xeus-octave)
    install(DIRECTORY ${XOCTAVE_SCRIPTS_DIR}
            DESTINATION ${XEUS_OCTAVE_SCRIPTS_BASEDIR}
            PATTERN "*.in" EXCLUDE)

    # Extra path for installing Jupyter kernelspec
    if (XEXTRA_JUPYTER_DATA_DIR)
        install(DIRECTORY ${KERNELSPEC_DIR}
                DESTINATION ${XEXTRA_JUPYTER_DATA_DIR}
                PATTERN "*.in" EXCLUDE)
    endif ()
endif ()

# Configure 'xeus-octaveConfig.cmake' for a build tree
set(XEUS_OCTAVE_CONFIG_CODE "####### Expanded from \@XEUS_OCTAVE_CONFIG_CODE\@ #######\n")
set(XEUS_OCTAVE_CONFIG_CODE "${XEUS_OCTAVE_CONFIG_CODE}set(CMAKE_MODULE_PATH \"${CMAKE_CURRENT_SOURCE_DIR}/cmake;\${CMAKE_MODULE_PATH}\")\n")
set(XEUS_OCTAVE_CONFIG_CODE "${XEUS_OCTAVE_CONFIG_CODE}##################################################")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${PROJECT_BINARY_DIR})

# Configure 'xeus-octaveConfig.cmake.in for an install tree
set(XEUS_OCTAVE_CONFIG_CODE "")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${XEUS_OCTAVE_CMAKECONFIG_INSTALL_DIR})

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                                 VERSION ${${PROJECT_NAME}_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
              DESTINATION ${XEUS_OCTAVE_CMAKECONFIG_INSTALL_DIR})

if (XEUS_OCTAVE_BUILD_SHARED)
    install(EXPORT ${PROJECT_NAME}-targets
            FILE ${PROJECT_NAME}Targets.cmake
            DESTINATION ${XEUS_OCTAVE_CMAKECONFIG_INSTALL_DIR})
endif ()
