# CMSIS, used header-only. SYSTEM include paths: warnings inside ST and Arm headers are not ours.
# system_stm32f4xx.c and ST's startup file are not compiled (drivers/stm32f407/startup/README.md).

set(_core_include   ${CMAKE_CURRENT_SOURCE_DIR}/cmsis_core/Include)
set(_device_include ${CMAKE_CURRENT_SOURCE_DIR}/cmsis_device_f4/Include)

if(NOT EXISTS ${_device_include}/stm32f407xx.h OR NOT EXISTS ${_core_include}/core_cm4.h)
  message(FATAL_ERROR "CMSIS is missing. Run: git submodule update --init")
endif()

add_library(nexus_cmsis INTERFACE)
target_include_directories(nexus_cmsis SYSTEM INTERFACE ${_core_include} ${_device_include})
target_compile_definitions(nexus_cmsis INTERFACE STM32F407xx)
