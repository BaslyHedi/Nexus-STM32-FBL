/*
 * Bootloader (Nexus-Physical-01-04) for the STM32H533RE.
 * Flash, SRAM and backup SRAM come from interface/linker/memory_map.ld (SW-04, SW-05, SW-06).
 *
 * The FBL is stored in the flash and EXECUTES FROM THE SRAM (SW-07), so a flash erase does
 * not stall it. Only the vector table and the start-up code are used from the flash:
 *
 *   flash  .isr_vector   vector table, read at reset
 *          .version      version record, 64 bytes at 0x08000400
 *          .boot_text   start-up routine: copies .vector_ram and .ram_image, sets VTOR
 *          .ram_image    LOAD ADDRESS of the code, constants and data that run from the SRAM
 *   SRAM   .vector_ram   copy of the vector table, aligned for VTOR
 *          .ram_image    code, constants and initialised data
 *          .bss          zero-initialised data
 *   backup SRAM  .handoff  hand-off record, never loaded and never cleared (SW-06)
 *
 * No heap is defined on purpose: a call to malloc or operator new fails to link.
 */

ENTRY(Reset_Handler)

INCLUDE memory_map.ld

/* Smallest stack the image must leave free at the top of the SRAM. */
_Min_Stack_Size = 2K;

/*
 * VTOR alignment for 149 vectors (16 system + 133 interrupts, RM0481 Table 153): the table is 596
 * bytes, so TBLOFF needs the next power of two, 1024 bytes (SW-07). To confirm in the Armv8-M
 * Architecture Reference Manual: it is not in RM0481.
 */
_Vector_Alignment = 1024;

MEMORY
{
  FLASH   (rx)  : ORIGIN = FBL_FLASH_ORIGIN, LENGTH = FBL_FLASH_LENGTH
  RAM     (rwx) : ORIGIN = SRAM_ORIGIN,      LENGTH = SRAM_LENGTH
  BKPSRAM (rw)  : ORIGIN = HANDOFF_ORIGIN,   LENGTH = HANDOFF_LENGTH
}

SECTIONS
{
  /* Flash: vector table first. The core reads the stack pointer and reset vector from here. */
  .isr_vector :
  {
    . = ALIGN(4);
    __isr_vector_load = .;
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } > FLASH

  /*
   * Flash: the version record, 64 bytes at a fixed offset (0x08000400), before the first code.
   * Mark it with section(".version"). Never copied to the SRAM. Not read by the core: the tools
   * read it from the image. The gap between the vector table and the record holds nothing.
   */
  .version (ORIGIN(FLASH) + VERSION_OFFSET) :
  {
    KEEP(*(.version))
  } > FLASH

  /*
   * Flash: code that runs before the copy. Mark it with section(".boot_text"). It must not
   * call, read or write anything that is in .ram_image. GCC places main in .text.startup,
   * which goes to the SRAM below, so this section is not .text*.
   */
  .boot_text :
  {
    . = ALIGN(4);
    KEEP(*(.boot_text))
    KEEP(*(.boot_text.*))
    . = ALIGN(4);
  } > FLASH

  /* Unwinding tables stay in the flash. With no exceptions they are not read. */
  .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } > FLASH
  .ARM : { __exidx_start = .; *(.ARM.exidx*) __exidx_end = .; } > FLASH

  /* Constructor tables stay in the flash. Empty for an image with no non-trivial globals (SW-03). */
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

  /* SRAM: the vector table copy. At the start of the SRAM, which is aligned far beyond 1024 bytes. */
  .vector_ram (NOLOAD) :
  {
    . = ALIGN(_Vector_Alignment);
    __vector_ram = .;
    . = . + SIZEOF(.isr_vector);
    . = ALIGN(4);
    __vector_ram_end = .;
  } > RAM

  /* SRAM at run time, flash at rest: code, constants and initialised data, copied as one block. */
  .ram_image :
  {
    . = ALIGN(4);
    __ram_image_start = .;
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame*)
    KEEP(*(.init))
    KEEP(*(.fini))
    . = ALIGN(4);
    *(.rodata*)
    . = ALIGN(4);
    *(.data*)
    . = ALIGN(4);
    __ram_image_end = .;
  } > RAM AT> FLASH

  __ram_image_load = LOADADDR(.ram_image);

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

  /*
   * Backup SRAM: the hand-off record. Mark it with section(".handoff"). NOLOAD, so the image holds
   * nothing for it, and nothing in the start-up clears it. The record carries its own magic and CRC.
   */
  .handoff (NOLOAD) :
  {
    . = ALIGN(4);
    __handoff_start = .;
    KEEP(*(.handoff))
    . = ALIGN(4);
    __handoff_end = .;
  } > BKPSRAM

  /* End of the image in the flash, for the image tool and for the size report. */
  __fbl_image_end = LOADADDR(.ram_image) + SIZEOF(.ram_image);

  /DISCARD/ :
  {
    *(.note.GNU-stack)
    *(.comment)
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}

ASSERT((__vector_ram & (_Vector_Alignment - 1)) == 0, "vector table copy is not aligned for VTOR")
ASSERT(SIZEOF(.vector_ram) == SIZEOF(.isr_vector), "vector table copy and original differ in size")
ASSERT(__isr_vector_load == ORIGIN(FLASH), "the vector table must be at the start of the FBL region")
ASSERT(SIZEOF(.isr_vector) <= VERSION_OFFSET, "the vector table overlaps the version record")
ASSERT(SIZEOF(.version) == VERSION_SIZE, "the version record is missing or is not 64 bytes")
ASSERT(ADDR(.version) == FBL_FLASH_ORIGIN + VERSION_OFFSET, "the version record is not at its fixed address")
ASSERT(SIZEOF(.handoff) <= HANDOFF_LENGTH, "hand-off record is larger than the region reserved for it")
