# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2026 Hedi Basly
#
# File:      src/CMakeLists.txt
# Brief:     The bootloader sources, added to the executable of each variant.
# Element:   Nexus-Physical-01-04 (bootloader)
# Decisions: none
#
# One file for the whole folder: paths are relative to src/.

foreach(_variant IN LISTS NEXUS_VARIANTS)
  target_sources(${NEXUS_TARGET_${_variant}} PRIVATE
    boot/main.cpp)
endforeach()
