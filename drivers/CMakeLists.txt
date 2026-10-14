# The drivers and the start-up code. One file for the whole folder: paths are relative to drivers/.

# Start-up code: an OBJECT library for the same reason as nexus_version, the linker script's ENTRY
# is the only reference. One per variant, compiled with that variant's options.
foreach(_variant IN LISTS NEXUS_VARIANTS)
  add_library(nexus_startup_${_variant} OBJECT stm32f407/startup/startup.cpp)
  target_link_libraries(nexus_startup_${_variant} PRIVATE nexus_options nexus_variant_${_variant} nexus_cmsis)
endforeach()

# The start-up code runs from the flash before .ram_image is copied. It must not call anything in
# .ram_image, and GCC turns a copy or clear loop into a call to memcpy or memset unless told not to.
set_source_files_properties(stm32f407/startup/startup.cpp PROPERTIES
  COMPILE_OPTIONS "-fno-tree-loop-distribute-patterns")
