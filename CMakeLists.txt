# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2026 Hedi Basly
#
# File:      CMakeLists.txt
# Brief:     Root of the build: wires the settings, the variants and the folders together.
# Element:   Nexus-Physical-01-04 (bootloader)
# Decisions: none

cmake_minimum_required(VERSION 3.25)

# Settings first: the project name and version come from them.
include(cmake/project-settings.cmake)

project(${NEXUS_PROJECT_NAME}
  VERSION ${NEXUS_VERSION_MAJOR}.${NEXUS_VERSION_MINOR}.${NEXUS_VERSION_PATCH}
  LANGUAGES CXX)

include(cmake/version.cmake)
include(cmake/compile-options.cmake)
include(cmake/post-build.cmake)

add_subdirectory(third_party)
add_subdirectory(version)
add_subdirectory(drivers)
add_subdirectory(cyphal)

# One executable per variant (project-settings.cmake). Created before src/, which adds its sources to each.
foreach(_variant IN LISTS NEXUS_VARIANTS)
  add_executable(${NEXUS_TARGET_${_variant}})
endforeach()

add_subdirectory(src)

foreach(_variant IN LISTS NEXUS_VARIANTS)
  set(_target ${NEXUS_TARGET_${_variant}})

  target_link_libraries(${_target} PRIVATE
    nexus_options
    nexus_variant_${_variant}
    nexus_startup_${_variant}
    nexus_drivers_${_variant}
    nexus_cyphal_${_variant}
    nexus_version_${_variant})

  set_target_properties(${_target} PROPERTIES
    OUTPUT_NAME ${NEXUS_OUTPUT_NAME_${_variant}}
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY ${NEXUS_OUTPUT_DIR})

  nexus_use_linker_script(${_target} ${NEXUS_OUTPUT_NAME_${_variant}})
endforeach()

nexus_post_build(${NEXUS_TARGET_release} SREC)
nexus_post_build(${NEXUS_TARGET_debug})
