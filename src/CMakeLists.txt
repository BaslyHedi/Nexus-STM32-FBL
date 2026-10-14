# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2026 Hedi Basly
#
# File:      src/CMakeLists.txt
# Brief:     The bootloader sources, added to the executable of each variant.
# Element:   Nexus-Physical-01-04 (bootloader)
# Decisions: none
#
# One file for the whole folder: paths are relative to src/.

# The sources. One source per line: a new source adds one line here.
set(_src_sources
  boot/main.cpp
  leds/player.cpp
)

foreach(_variant IN LISTS NEXUS_VARIANTS)
  target_sources(${NEXUS_TARGET_${_variant}} PRIVATE
    ${_src_sources})
  target_include_directories(${NEXUS_TARGET_${_variant}} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})
endforeach()
