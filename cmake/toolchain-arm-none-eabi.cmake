# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2026 Hedi Basly
#
# File:      cmake/toolchain-arm-none-eabi.cmake
# Brief:     Cross-compilation for the STM32H533 (Cortex-M33, no FPU used).
# Element:   Nexus-Physical-01-04 (bootloader)
# Decisions: SW-05
#
# Passed with -DCMAKE_TOOLCHAIN_FILE, which CMakePresets.json does.

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(NEXUS_TOOLCHAIN_PREFIX arm-none-eabi-)

find_program(CMAKE_CXX_COMPILER ${NEXUS_TOOLCHAIN_PREFIX}g++ REQUIRED)

# The tools of the same toolchain, found next to the compiler.
get_filename_component(_nexus_bin_dir ${CMAKE_CXX_COMPILER} DIRECTORY)
foreach(_tool OBJCOPY SIZE READELF)
  string(TOLOWER ${_tool} _name)
  find_program(NEXUS_${_tool} ${NEXUS_TOOLCHAIN_PREFIX}${_name}
    HINTS ${_nexus_bin_dir} NO_DEFAULT_PATH REQUIRED)
endforeach()

# There is no host to link a test program against: build a static library instead.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Only the CPU flags live here, every project target needs them. The rest is in compile-options.cmake.
set(NEXUS_CPU_FLAGS -mcpu=cortex-m33 -mthumb -mfloat-abi=soft)
string(JOIN " " _nexus_cpu_flags ${NEXUS_CPU_FLAGS})
set(CMAKE_CXX_FLAGS_INIT ${_nexus_cpu_flags})
set(CMAKE_EXE_LINKER_FLAGS_INIT ${_nexus_cpu_flags})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
