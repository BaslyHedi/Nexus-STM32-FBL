# Steps after the link: the .init_array check, the SREC files for the ST-LINK and the size report.

# FBL_Nexus.srec and FBL_Nexus_v1_00-01-00.srec: same content, two names. objcopy runs twice
# because the SREC header record (S0) carries the output file name. It runs in the output folder
# with a relative name: given a full path, the header would hold the path of the build machine.
# S3 records (32-bit addresses) are forced; the STM32 flash is at 0x08000000.
function(nexus_post_build target)
  set(_elf "$<TARGET_FILE:${target}>")

  # The old SREC files are removed and the check comes before the new ones: a failed build must
  # not leave an SREC behind that could be flashed, neither a new one nor one of the last good build.
  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -f "${NEXUS_OUTPUT_NAME}.srec" "${NEXUS_OUTPUT_NAME_VERSIONED}.srec"
    COMMAND ${CMAKE_COMMAND}
      -DREADELF=${NEXUS_READELF} -DELF=${_elf}
      -P "${CMAKE_SOURCE_DIR}/cmake/check-init-array.cmake"
    COMMAND ${NEXUS_OBJCOPY} -O srec --srec-forceS3 ${_elf} "${NEXUS_OUTPUT_NAME}.srec"
    COMMAND ${NEXUS_OBJCOPY} -O srec --srec-forceS3 ${_elf} "${NEXUS_OUTPUT_NAME_VERSIONED}.srec"
    COMMAND ${NEXUS_SIZE} ${_elf}
    WORKING_DIRECTORY ${NEXUS_OUTPUT_DIR}
    COMMENT ".init_array check, SREC files, size"
    VERBATIM)
endfunction()
