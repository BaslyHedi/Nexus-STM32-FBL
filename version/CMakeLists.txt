# An OBJECT library, not a static one: nothing references the record, so an archive would
# leave version.cpp.o out of the link. Linking an OBJECT library adds its objects to the executable.
# One per variant, compiled with that variant's options.
foreach(_variant IN LISTS NEXUS_VARIANTS)
  add_library(nexus_version_${_variant} OBJECT version.cpp)
  target_include_directories(nexus_version_${_variant} PUBLIC ${NEXUS_GENERATED_DIR})
  target_link_libraries(nexus_version_${_variant} PRIVATE nexus_options nexus_variant_${_variant})
endforeach()
