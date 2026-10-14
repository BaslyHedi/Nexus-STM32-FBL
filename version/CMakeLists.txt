# An OBJECT library, not a static one: nothing references the record, so an archive would
# leave version.cpp.o out of the link. Linking an OBJECT library adds its objects to the executable.
add_library(nexus_version OBJECT version.cpp)
target_include_directories(nexus_version PUBLIC ${NEXUS_GENERATED_DIR})
target_link_libraries(nexus_version PRIVATE nexus_options)
