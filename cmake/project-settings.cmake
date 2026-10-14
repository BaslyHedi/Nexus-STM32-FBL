# Project settings: names, version and file names. Included before project().
# Nothing else in the build hard-codes these values.

# Software identity. The record in the image is built from these (version/README.md).
set(NEXUS_SW_TYPE          "FBL")    # FBL or APP, 3 characters
set(NEXUS_PLATFORM_NAME    "Nexus")  # 5 characters
set(NEXUS_PLATFORM_VERSION "v1")     # 2 characters

# Software version, each part 0 to 99.
set(NEXUS_VERSION_MAJOR 0)
set(NEXUS_VERSION_MINOR 1)
set(NEXUS_VERSION_PATCH 0)

# CMake project and executable target.
set(NEXUS_PROJECT_NAME "Nexus-STM32-FBL")
set(NEXUS_TARGET       "fbl")

# Output files. version.cmake adds the versioned name:
# FBL_Nexus_v1_00-01-00 next to FBL_Nexus.
set(NEXUS_OUTPUT_NAME "${NEXUS_SW_TYPE}_${NEXUS_PLATFORM_NAME}")
set(NEXUS_OUTPUT_DIR  "${CMAKE_BINARY_DIR}/output")

# Linker script, relative to the repository root. memory_map.ld is included by it and found with -L.
set(NEXUS_LINKER_SCRIPT   "linker/fbl.ld")
set(NEXUS_LINKER_INC_DIR  "interface/linker")
