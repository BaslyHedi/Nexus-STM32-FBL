# The bootloader sources, added to the executable of each variant. One file for the whole folder:
# paths are relative to src/.
foreach(_variant IN LISTS NEXUS_VARIANTS)
  target_sources(${NEXUS_TARGET_${_variant}} PRIVATE
    boot/main.cpp)
endforeach()
