/*
 * Bootloader (Nexus-Physical-01-04) for the STM32F407VGT6.
 * Flash and RAM come from interface/linker/memory_map.ld (SW-04, SW-05).
 * No heap is defined on purpose: a call to malloc or operator new fails to link.
 */

ENTRY(Reset_Handler)

INCLUDE memory_map.ld

/* Smallest stack the image must leave free at the top of the SRAM. */
_Min_Stack_Size = 2K;

MEMORY
{
  FLASH (rx)  : ORIGIN = FBL_FLASH_ORIGIN, LENGTH = FBL_FLASH_LENGTH
  RAM   (rwx) : ORIGIN = SRAM_ORIGIN,      LENGTH = SRAM_LENGTH
}

SECTIONS
{
  /* Vector table first: the core reads the stack pointer and reset vector from here. */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } > FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame*)
    KEEP(*(.init))
    KEEP(*(.fini))
    . = ALIGN(4);
  } > FLASH

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata*)
    . = ALIGN(4);
  } > FLASH

  .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } > FLASH
  .ARM : { __exidx_start = .; *(.ARM.exidx*) __exidx_end = .; } > FLASH

  /* Constructors and destructors. Empty for an image with no non-trivial globals (SW-03). */
  .preinit_array :
  {
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*(.preinit_array*))
    PROVIDE_HIDDEN(__preinit_array_end = .);
  } > FLASH

  .init_array :
  {
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array*))
    PROVIDE_HIDDEN(__init_array_end = .);
  } > FLASH

  .fini_array :
  {
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array*))
    PROVIDE_HIDDEN(__fini_array_end = .);
  } > FLASH

  /* Initialised data: lives in the SRAM at run time, stored in the flash after the code. */
  __data_load = LOADADDR(.data);

  .data :
  {
    . = ALIGN(4);
    __data_start = .;
    *(.data*)
    . = ALIGN(4);
    __data_end = .;
  } > RAM AT> FLASH

  .bss (NOLOAD) :
  {
    . = ALIGN(4);
    __bss_start = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end = .;
  } > RAM

  /* The stack grows down from the end of the SRAM. This section only checks it has room. */
  ._user_stack (NOLOAD) :
  {
    . = ALIGN(8);
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } > RAM

  __stack_top = ORIGIN(RAM) + LENGTH(RAM);

  /* End of the image in the flash, for the image tool and for the size report. */
  __fbl_image_end = LOADADDR(.data) + SIZEOF(.data);

  /DISCARD/ :
  {
    *(.note.GNU-stack)
    *(.comment)
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
