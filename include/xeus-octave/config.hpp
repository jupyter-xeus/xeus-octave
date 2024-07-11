/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_CONFIG_HPP
#define XEUS_OCTAVE_CONFIG_HPP

// Project version
#define XEUS_OCTAVE_VERSION_MAJOR 0
#define XEUS_OCTAVE_VERSION_MINOR 2
#define XEUS_OCTAVE_VERSION_PATCH 0

// Composing the version string from major, minor and patch
#define XEUS_OCTAVE_CONCATENATE(A, B) XEUS_OCTAVE_CONCATENATE_IMPL(A, B)
#define XEUS_OCTAVE_CONCATENATE_IMPL(A, B) A##B
#define XEUS_OCTAVE_STRINGIFY(a) XEUS_OCTAVE_STRINGIFY_IMPL(a)
#define XEUS_OCTAVE_STRINGIFY_IMPL(a) #a

#define XEUS_OCTAVE_VERSION                                                                                          \
  XEUS_OCTAVE_STRINGIFY(XEUS_OCTAVE_CONCATENATE(                                                                     \
    XEUS_OCTAVE_VERSION_MAJOR,                                                                                       \
    XEUS_OCTAVE_CONCATENATE(                                                                                         \
        ., XEUS_OCTAVE_CONCATENATE(XEUS_OCTAVE_VERSION_MINOR, XEUS_OCTAVE_CONCATENATE(., XEUS_OCTAVE_VERSION_PATCH)) \
    )                                                                                                                \
  ))

#ifdef _WIN32
#ifdef XEUS_OCTAVE_EXPORTS
#define XEUS_OCTAVE_API __declspec(dllexport)
#else
#define XEUS_OCTAVE_API __declspec(dllimport)
#endif
#else
#define XEUS_OCTAVE_API
#endif

#endif  // XEUS_OCTAVE_CONFIG_HPP
