cmake_minimum_required(VERSION 3.25)

# Settings first: the project name and version come from them.
include(cmake/project-settings.cmake)

project(${NEXUS_PROJECT_NAME}
  VERSION ${NEXUS_VERSION_MAJOR}.${NEXUS_VERSION_MINOR}.${NEXUS_VERSION_PATCH}
  LANGUAGES CXX)

include(cmake/version.cmake)
include(cmake/compile-options.cmake)
include(cmake/post-build.cmake)

add_subdirectory(third_party)
add_subdirectory(version)
add_subdirectory(drivers/stm32f407/startup)

add_executable(${NEXUS_TARGET})
add_subdirectory(src/boot)

target_link_libraries(${NEXUS_TARGET} PRIVATE
  nexus_options
  nexus_startup
  nexus_version)

set_target_properties(${NEXUS_TARGET} PROPERTIES
  OUTPUT_NAME ${NEXUS_OUTPUT_NAME}
  SUFFIX ".elf"
  RUNTIME_OUTPUT_DIRECTORY ${NEXUS_OUTPUT_DIR})

nexus_use_linker_script(${NEXUS_TARGET})
nexus_post_build(${NEXUS_TARGET})
