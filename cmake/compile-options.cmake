# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2026 Hedi Basly
#
# File:      cmake/compile-options.cmake
# Brief:     Compiler and linker options shared by every target of the image.
# Element:   Nexus-Physical-01-04 (bootloader)
# Decisions: SW-03
#
# Creates the INTERFACE library nexus_options; a target takes the options by linking it. The CPU
# flags are in the toolchain file.

option(NEXUS_WARNINGS_AS_ERRORS "Treat warnings as errors" ON)

# -std=c++17 and not -std=gnu++17. GCC 14 defaults to gnu++17, so without this CMake adds no -std flag at all.
# The variable sets CXX_EXTENSIONS on every target created after this line.
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(nexus_options INTERFACE)

# C++17 (SW-03).
target_compile_features(nexus_options INTERFACE cxx_std_17)

target_compile_options(nexus_options INTERFACE
  # Freestanding subset (SW-03): no exceptions, no RTTI, no allocation, no atexit.
  -ffreestanding
  -fno-exceptions
  -fno-rtti
  -fno-threadsafe-statics
  -fno-use-cxa-atexit
  -fno-unwind-tables
  -fno-asynchronous-unwind-tables
  # One section per function and object, so --gc-sections can drop what is unused.
  -ffunction-sections
  -fdata-sections
  # Warnings.
  -Wall
  -Wextra
  -Wpedantic
  -Wconversion
  -Wsign-conversion
  -Wshadow
  -Wold-style-cast
  -Wcast-align
  -Wnon-virtual-dtor
  -Woverloaded-virtual
  -Wmissing-declarations
  -Wundef
  -Wuseless-cast
  $<$<BOOL:${NEXUS_WARNINGS_AS_ERRORS}>:-Werror>
)

# -nostdlib: no C library and no C++ runtime. libgcc stays for the compiler's own helpers (division).
# A call to malloc or operator new fails to link, which is the intent.
# The four memory functions GCC may call are in drivers/runtime/memory.cpp.
target_link_options(nexus_options INTERFACE
  -nostartfiles
  -nostdlib
  -Wl,--gc-sections
  -Wl,--print-memory-usage
)

# binutils 2.39 and newer warn about the RWX segment of the SRAM image (it is on purpose, the code runs
# from the SRAM) and know the option that silences it. Older ones stop at an unknown option.
# Asked of the linker itself: the try-compile above is a static library and would not test a link option.
execute_process(COMMAND ${CMAKE_CXX_COMPILER} -Wl,--help
  OUTPUT_VARIABLE _nexus_ld_help ERROR_QUIET)
if(_nexus_ld_help MATCHES "no-warn-rwx-segments")
  target_link_options(nexus_options INTERFACE -Wl,--no-warn-rwx-segments)
endif()
target_link_libraries(nexus_options INTERFACE gcc)

# The optimisation is set per variant, never by the build type: CMAKE_BUILD_TYPE adds its flags to
# every target, so MinSizeRel would put -Os on the debug variant too.
if(CMAKE_BUILD_TYPE)
  message(FATAL_ERROR "CMAKE_BUILD_TYPE is '${CMAKE_BUILD_TYPE}', it must be empty: each variant sets "
    "its own optimisation (cmake/README.md). A build tree from before the variants needs: cmake --preset fbl --fresh")
endif()

# One INTERFACE library per variant of NEXUS_VARIANTS, linked next to nexus_options.
# Both carry -g3: debug information goes into the .elf only, never into the SREC, and GCC
# generates the same code with or without -g.
add_library(nexus_variant_release INTERFACE)
target_compile_options(nexus_variant_release INTERFACE -Os -g3)
target_compile_definitions(nexus_variant_release INTERFACE NDEBUG)

add_library(nexus_variant_debug INTERFACE)
target_compile_options(nexus_variant_debug INTERFACE -Og -g3)

# Linker script and its include path, in this order: ld only searches the -L paths
# it has already read when it opens the script (linker/README.md).
# Called by the executable target, because the script belongs to the image. The map file takes output_name.
function(nexus_use_linker_script target output_name)
  target_link_options(${target} PRIVATE
    "-L${CMAKE_SOURCE_DIR}/${NEXUS_LINKER_INC_DIR}"
    "-T${CMAKE_SOURCE_DIR}/${NEXUS_LINKER_SCRIPT}"
    "-Wl,-Map=${NEXUS_OUTPUT_DIR}/${output_name}.map"
  )
  # Relink when the script or the memory map changes.
  set_property(TARGET ${target} APPEND PROPERTY LINK_DEPENDS
    "${CMAKE_SOURCE_DIR}/${NEXUS_LINKER_SCRIPT}"
    "${CMAKE_SOURCE_DIR}/${NEXUS_LINKER_INC_DIR}/memory_map.ld")
endfunction()
