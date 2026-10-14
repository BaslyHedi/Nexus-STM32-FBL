# Steps after the link: the .init_array check, the SREC files for the ST-LINK and the size report.
# Every variant gets the check and the size report. Only a target called with SREC gets the SREC files:
# the release image is what goes on a board, and an SREC of the debug image next to it could be
# flashed by mistake. The debug image is flashed from its .elf.

# FBL_Nexus.srec and FBL_Nexus_v1_00-01-00.srec: same content, two names. objcopy runs twice
# because the SREC header record (S0) carries the output file name. It runs in the output folder
# with a relative name: given a full path, the header would hold the path of the build machine.
# S3 records (32-bit addresses) are forced; the STM32 flash is at 0x08000000.
function(nexus_post_build target)
  cmake_parse_arguments(PARSE_ARGV 1 _arg "SREC" "" "")
  set(_elf "$<TARGET_FILE:${target}>")

  set(_check
    COMMAND ${CMAKE_COMMAND}
      -DREADELF=${NEXUS_READELF} -DELF=${_elf}
      -P "${CMAKE_SOURCE_DIR}/cmake/check-init-array.cmake")

  if(_arg_SREC)
    # The old SREC files are removed and the check comes before the new ones: a failed build must
    # not leave an SREC behind that could be flashed, neither a new one nor one of the last good build.
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E rm -f "${NEXUS_OUTPUT_NAME}.srec" "${NEXUS_OUTPUT_NAME_VERSIONED}.srec"
      ${_check}
      COMMAND ${NEXUS_OBJCOPY} -O srec --srec-forceS3 ${_elf} "${NEXUS_OUTPUT_NAME}.srec"
      COMMAND ${NEXUS_OBJCOPY} -O srec --srec-forceS3 ${_elf} "${NEXUS_OUTPUT_NAME_VERSIONED}.srec"
      COMMAND ${NEXUS_SIZE} ${_elf}
      WORKING_DIRECTORY ${NEXUS_OUTPUT_DIR}
      COMMENT ".init_array check, SREC files, size"
      VERBATIM)
  else()
    add_custom_command(TARGET ${target} POST_BUILD
      ${_check}
      COMMAND ${NEXUS_SIZE} ${_elf}
      WORKING_DIRECTORY ${NEXUS_OUTPUT_DIR}
      COMMENT ".init_array check, size"
      VERBATIM)
  endif()
endfunction()
