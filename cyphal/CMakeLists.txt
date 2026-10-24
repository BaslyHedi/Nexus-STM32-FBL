# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2026 Hedi Basly
#
# File:      cyphal/CMakeLists.txt
# Brief:     The in-house Cyphal stack, one OBJECT library per variant.
# Element:   Nexus-Physical-01-04 (bootloader)
# Decisions: SW-02, SW-03
#
# One file for the whole folder: paths are relative to cyphal/. The library links nothing else of
# this repository, so the stack can move out later (cyphal/README.md). Its public include
# directory is cyphal/include: headers are included as nexus/cyphal/<name>.hpp.

# The sources. One source per line: a new source adds one line here.
set(_cyphal_sources
  tests/transfer_crc_checks.cpp
)

foreach(_variant IN LISTS NEXUS_VARIANTS)
  add_library(nexus_cyphal_${_variant} OBJECT ${_cyphal_sources})
  target_include_directories(nexus_cyphal_${_variant} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
  target_link_libraries(nexus_cyphal_${_variant} PRIVATE nexus_options nexus_variant_${_variant})
endforeach()
