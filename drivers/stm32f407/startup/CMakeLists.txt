# OBJECT library for the same reason as nexus_version: the linker script's ENTRY is the only reference.
add_library(nexus_startup OBJECT startup.cpp)
target_link_libraries(nexus_startup PRIVATE nexus_options nexus_cmsis)

# The start-up code runs from the flash before .ram_image is copied. It must not call anything in
# .ram_image, and GCC turns a copy or clear loop into a call to memcpy or memset unless told not to.
set_source_files_properties(startup.cpp PROPERTIES
  COMPILE_OPTIONS "-fno-tree-loop-distribute-patterns")
