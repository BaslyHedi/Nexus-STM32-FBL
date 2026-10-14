# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2026 Hedi Basly
#
# File:      cmake/project-settings.cmake
# Brief:     Project settings: names, version, variants and file names.
# Element:   Nexus-Physical-01-04 (bootloader)
# Decisions: SW-08
#
# Included before project(). Nothing else in the build hard-codes these values.

# Software identity. The record in the image is built from these (version/README.md).
set(NEXUS_SW_TYPE          "FBL")    # FBL or APP, 3 characters
set(NEXUS_PLATFORM_NAME    "Nexus")  # 5 characters
set(NEXUS_PLATFORM_VERSION "v1")     # 2 characters

# Software version, each part 0 to 99.
set(NEXUS_VERSION_MAJOR 0)
set(NEXUS_VERSION_MINOR 1)
set(NEXUS_VERSION_PATCH 0)

# CMake project.
set(NEXUS_PROJECT_NAME "Nexus-STM32-FBL")

# Build variants: the same sources built once per variant, side by side in one build tree.
# Their compile options are in compile-options.cmake.
#   release  the image that ships, the only one with SREC files
#   debug    for the debugger, less optimised; flashed from its .elf
set(NEXUS_VARIANTS release debug)

# Executable target of each variant.
set(NEXUS_TARGET_release "fbl")
set(NEXUS_TARGET_debug   "fbl_debug")

# Output files of each variant. version.cmake adds the versioned name of the release image:
# FBL_Nexus_v1_00-01-00 next to FBL_Nexus.
set(NEXUS_OUTPUT_NAME         "${NEXUS_SW_TYPE}_${NEXUS_PLATFORM_NAME}")
set(NEXUS_OUTPUT_NAME_release "${NEXUS_OUTPUT_NAME}")
set(NEXUS_OUTPUT_NAME_debug   "${NEXUS_OUTPUT_NAME}_debug")
set(NEXUS_OUTPUT_DIR          "${CMAKE_BINARY_DIR}/output")

# Linker script, relative to the repository root. memory_map.ld is included by it and found with -L.
set(NEXUS_LINKER_SCRIPT   "linker/fbl.ld")
set(NEXUS_LINKER_INC_DIR  "interface/linker")
