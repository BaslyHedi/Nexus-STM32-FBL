# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2026 Hedi Basly
#
# File:      cmake/version.cmake
# Brief:     Checks the version settings, derives the strings and generates version.hpp.
# Element:   Nexus-Physical-01-04 (bootloader)
# Decisions: SW-08
#
# Checks the settings of project-settings.cmake against the layout of the version record, derives
# the strings, and generates version.hpp from version/version.hpp.in. The record is 64 bytes at a
# fixed place in the image (version/README.md).

# The layout of the record, written once, here. The checks below and version.hpp.in take it from
# these variables. NEXUS_RECORD_SIZE is also VERSION_SIZE in interface/linker/memory_map.ld: that one
# is outside CMake, and fbl.ld fails the link if the section is not that size.
set(NEXUS_RECORD_SIZE             64)
set(NEXUS_RECORD_TYPE_SIZE        3)   # FBL or APP
set(NEXUS_RECORD_PLATFORM_SIZE    5)
set(NEXUS_RECORD_PLATFORM_VER_SIZE 2)
set(NEXUS_RECORD_SW_VERSION_SIZE  8)   # AA-BB-CC
# Three separators and the NUL are in the record too; what is left is reserved.
math(EXPR NEXUS_RECORD_RESERVED_SIZE
  "${NEXUS_RECORD_SIZE} - ${NEXUS_RECORD_TYPE_SIZE} - ${NEXUS_RECORD_PLATFORM_SIZE} - ${NEXUS_RECORD_PLATFORM_VER_SIZE} - ${NEXUS_RECORD_SW_VERSION_SIZE} - 4")

# A field is letters and digits only, and exactly its width: the value goes into a string literal and
# into file names, "_" is the separator, and the layout is fixed.
function(nexus_check_field setting width)
  string(LENGTH "${${setting}}" _length)
  if(NOT "${${setting}}" MATCHES "^[A-Za-z0-9]+$" OR NOT _length EQUAL ${width})
    message(FATAL_ERROR "${setting} must be ${width} letters or digits (the record has a fixed layout), got '${${setting}}'")
  endif()
endfunction()

nexus_check_field(NEXUS_SW_TYPE ${NEXUS_RECORD_TYPE_SIZE})
if(NOT NEXUS_SW_TYPE MATCHES "^(FBL|APP)$")
  message(FATAL_ERROR "NEXUS_SW_TYPE must be FBL or APP, got '${NEXUS_SW_TYPE}'")
endif()

nexus_check_field(NEXUS_PLATFORM_NAME    ${NEXUS_RECORD_PLATFORM_SIZE})
nexus_check_field(NEXUS_PLATFORM_VERSION ${NEXUS_RECORD_PLATFORM_VER_SIZE})

# Each version part is written with two digits: 0 -> 00, 12 -> 12.
foreach(_part MAJOR MINOR PATCH)
  set(_value "${NEXUS_VERSION_${_part}}")
  if(NOT _value MATCHES "^(0|[1-9][0-9]?)$")
    message(FATAL_ERROR "NEXUS_VERSION_${_part} must be a number from 0 to 99, got '${_value}'")
  endif()
  string(REGEX REPLACE "^([0-9])$" "0\\1" NEXUS_VERSION_${_part}_2D "${_value}")
endforeach()

set(NEXUS_SW_VERSION_STRING
  "${NEXUS_VERSION_MAJOR_2D}-${NEXUS_VERSION_MINOR_2D}-${NEXUS_VERSION_PATCH_2D}")
string(LENGTH "${NEXUS_SW_VERSION_STRING}" _sw_version_length)
if(NOT _sw_version_length EQUAL ${NEXUS_RECORD_SW_VERSION_SIZE})
  message(FATAL_ERROR "the software version '${NEXUS_SW_VERSION_STRING}' is not ${NEXUS_RECORD_SW_VERSION_SIZE} characters")
endif()
set(NEXUS_RECORD_STRING
  "${NEXUS_SW_TYPE}_${NEXUS_PLATFORM_NAME}_${NEXUS_PLATFORM_VERSION}_${NEXUS_SW_VERSION_STRING}")
set(NEXUS_OUTPUT_NAME_VERSIONED
  "${NEXUS_OUTPUT_NAME}_${NEXUS_PLATFORM_VERSION}_${NEXUS_SW_VERSION_STRING}")

# Generated into the build tree and never committed. Included as "version/version.hpp".
set(NEXUS_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")
configure_file(
  "${CMAKE_SOURCE_DIR}/version/version.hpp.in"
  "${NEXUS_GENERATED_DIR}/version/version.hpp"
  @ONLY)
