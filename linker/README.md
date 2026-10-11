# Linker scripts

`fbl.ld` links the bootloader to be **stored in the FBL flash region and executed from the SRAM** (SW-04, SW-05, SW-07), with the hand-off record in the backup SRAM (SW-06). `updater.ld` follows when the FBL-Updater is written. Both take their addresses from [`../interface/linker/memory_map.ld`](../interface/linker/memory_map.ld).

**Order on the link line:** `-L interface/linker` must come before `-T linker/fbl.ld`.

## Where each section lives

| Section | At rest (flash) | At run time | Filled by |
|---|---|---|---|
| `.isr_vector` | `0x08000000` | read at reset only | the vector table |
| `.boot_text` | after the vector table | runs from the flash | the start-up routine, marked `section(".boot_text")` |
| `.ram_image` | stored after `.boot_text` | `.text`, `.rodata`, `.data` in the SRAM | copied at start-up |
| `.vector_ram` | none | SRAM, 512-byte aligned, `VTOR` points here | copy of `.isr_vector` |
| `.bss` | none | SRAM | cleared at start-up |
| `.handoff` | none | backup SRAM `0x40024000`, 512 bytes | the application, marked `section(".handoff")` |

## What the start-up routine has to do

In `.boot_text`, using no function, constant or global from `.ram_image`: copy `[__ram_image_load ...]` to `[__ram_image_start, __ram_image_end)`, copy the vector table `[__isr_vector_load ...]` to `[__vector_ram, __vector_ram_end)`, write `__vector_ram` to `SCB->VTOR`, clear `[__bss_start, __bss_end)`, set the stack pointer to `__stack_top`, call the constructors if there are any, call `main`.

GCC places `main` in `.text.startup`, which the linker script sends to the SRAM. Do not mark it `.boot_text`.

## What the linker checks

Checked by linking stubs on the host:

- A flash, SRAM or backup SRAM overflow fails the link.
- At least 2 KB of stack is reserved, or the link fails.
- The vector table copy is aligned and the same size as the original, and the table sits at the start of the FBL region.
- There is no heap. A call to `malloc` or `operator new` fails to link.
- `.init_array` has start and end symbols, so an empty array can be checked in the map file (SW-03).
- `__fbl_image_end` marks the end of the image in the flash, for the image tool.

The 512-byte vector alignment is the minimum of PM0214 Rev 10, section 4.4.4: 128 words, for the 98 entries of the F407 (RM0090 Rev 22). The vector table copy must be complete before `VTOR` is written (PM0214 section 4.3.10).
