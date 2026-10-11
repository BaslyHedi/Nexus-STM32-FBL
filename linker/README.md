# Linker scripts

`fbl.ld` links the bootloader for the FBL region (SW-04) and the main SRAM (SW-05). `updater.ld` follows when the FBL-Updater is written. Both take their addresses from [`../interface/linker/memory_map.ld`](../interface/linker/memory_map.ld).

**Order on the link line:** `-L interface/linker` must come before `-T linker/fbl.ld`.

What `fbl.ld` guarantees, checked by linking a stub on the host:

- The vector table is first, at `0x08000000`.
- `.data` is stored in the flash after the code and copied to the SRAM at startup (`__data_load`, `__data_start`, `__data_end`).
- `.bss` is cleared at startup (`__bss_start`, `__bss_end`).
- The stack grows down from `__stack_top` (`0x20020000`), and at least 2 KB of it is reserved or the link fails.
- A flash or SRAM overflow fails the link.
- There is no heap. A call to `malloc` or `operator new` fails to link.
- `.init_array` has start and end symbols, so an empty array can be checked in the map file (SW-03).
- `__fbl_image_end` marks the end of the image in the flash, for the image tool.

Startup code and the hand-off block are not in this script. The hand-off location is undecided, see SW-05.
