# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2026 Hedi Basly
#
# File:      drivers/CMakeLists.txt
# Brief:     The start-up code, the runtime support and the drivers, one OBJECT library each per variant.
# Element:   Nexus-Physical-01-04 (bootloader)
# Decisions: SW-07
#
# One file for the whole folder: paths are relative to drivers/.

# Start-up code and runtime support: an OBJECT library for the same reason as the version record:
# nothing in the source calls them by name (the linker script's ENTRY, and the calls GCC emits).
# One per variant, compiled with that variant's options.
foreach(_variant IN LISTS NEXUS_VARIANTS)
  add_library(nexus_startup_${_variant} OBJECT
    startup/startup.cpp
    runtime/memory.cpp)
  target_link_libraries(nexus_startup_${_variant} PRIVATE nexus_options nexus_variant_${_variant} nexus_cmsis)
endforeach()

# The start-up code runs from the flash before .ram_image is copied. It must not call anything in
# .ram_image, and GCC turns a copy or clear loop into a call to memcpy or memset unless told not to.
# memory.cpp needs the same option, or memset would become a call to itself.
set_source_files_properties(startup/startup.cpp runtime/memory.cpp PROPERTIES
  COMPILE_OPTIONS "-fno-tree-loop-distribute-patterns")

# The drivers. One source per line: a new driver adds one line here.
set(_driver_sources
  stm32f407/leds/leds.cpp
  stm32f407/tick/tick.cpp
)

# The drivers, one OBJECT library per variant. OBJECT and not STATIC: a handler only overrides the
# weak alias of the start-up code, it never resolves an undefined symbol, so a static library would
# leave out a driver that is only an interrupt handler. The CMSIS headers stay PRIVATE: src/ sees
# the driver headers and not the registers.
foreach(_variant IN LISTS NEXUS_VARIANTS)
  add_library(nexus_drivers_${_variant} OBJECT ${_driver_sources})
  target_include_directories(nexus_drivers_${_variant} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(nexus_drivers_${_variant} PRIVATE nexus_options nexus_variant_${_variant} nexus_cmsis)
endforeach()
