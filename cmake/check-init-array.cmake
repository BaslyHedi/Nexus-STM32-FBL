# Fails the build when the image has constructors to run (SW-03): the constructor tables have to be empty.
# Run as: cmake -DREADELF=<readelf> -DELF=<elf> -P check-init-array.cmake
#
# Fails closed: a table that is named in the readelf output but whose line cannot be read is an error,
# not an empty table. A table missing altogether is empty.

execute_process(
  COMMAND ${READELF} --section-headers --wide ${ELF}
  OUTPUT_VARIABLE _sections
  RESULT_VARIABLE _result)
if(NOT _result EQUAL 0)
  message(FATAL_ERROR "readelf failed on ${ELF}")
endif()

# .preinit_array and .init_array are run by Reset_Main; .ctors is the older form of the same thing.
# A section line: [ n] .init_array INIT_ARRAY <addr> <off> <size> ...
foreach(_name .preinit_array .init_array .ctors)
  string(REPLACE "." "\\." _pattern "${_name}")
  if(_sections MATCHES "${_pattern}( |\n)")
    if(NOT _sections MATCHES "${_pattern} +[A-Z_]+ +[0-9a-f]+ +[0-9a-f]+ +([0-9a-f]+)")
      message(FATAL_ERROR "${_name} is in the readelf output but its size could not be read")
    endif()
    math(EXPR _size "0x${CMAKE_MATCH_1}")
    if(NOT _size EQUAL 0)
      message(FATAL_ERROR "${_name} holds ${_size} bytes: a global has a constructor to run (SW-03)")
    endif()
  endif()
endforeach()
message(STATUS "The constructor tables are empty")
