# Checks the settings of project-settings.cmake against the layout of the version record,
# derives the strings, and generates version.hpp from version/version.hpp.in.
# The record is 64 bytes at a fixed place in the image (version/README.md).

if(NOT NEXUS_SW_TYPE MATCHES "^(FBL|APP)$")
  message(FATAL_ERROR "NEXUS_SW_TYPE must be FBL or APP, got '${NEXUS_SW_TYPE}'")
endif()

string(LENGTH "${NEXUS_PLATFORM_NAME}" _name_length)
if(NOT _name_length EQUAL 5)
  message(FATAL_ERROR "NEXUS_PLATFORM_NAME must be 5 characters (the record has a fixed layout), got '${NEXUS_PLATFORM_NAME}'")
endif()

string(LENGTH "${NEXUS_PLATFORM_VERSION}" _platform_version_length)
if(NOT _platform_version_length EQUAL 2)
  message(FATAL_ERROR "NEXUS_PLATFORM_VERSION must be 2 characters (the record has a fixed layout), got '${NEXUS_PLATFORM_VERSION}'")
endif()

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
