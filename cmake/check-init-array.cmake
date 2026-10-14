# Fails the build when the image has constructors to run (SW-03): .init_array has to be empty.
# Run as: cmake -DREADELF=<readelf> -DELF=<elf> -P check-init-array.cmake

execute_process(
  COMMAND ${READELF} --section-headers --wide ${ELF}
  OUTPUT_VARIABLE _sections
  RESULT_VARIABLE _result)
if(NOT _result EQUAL 0)
  message(FATAL_ERROR "readelf failed on ${ELF}")
endif()

# A section line: [ n] .init_array INIT_ARRAY <addr> <off> <size> ...
# The section may be missing altogether, which is also an empty array.
if(_sections MATCHES "\\.init_array +INIT_ARRAY +[0-9a-f]+ +[0-9a-f]+ +([0-9a-f]+)")
  math(EXPR _size "0x${CMAKE_MATCH_1}")
  if(NOT _size EQUAL 0)
    message(FATAL_ERROR ".init_array holds ${_size} bytes: a global has a constructor to run (SW-03)")
  endif()
endif()
message(STATUS ".init_array is empty")
