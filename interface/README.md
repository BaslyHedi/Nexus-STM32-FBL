# Interface

The contract between the bootloader and what it loads, and between this repository and its consumers. Plain C headers (`.h`), so the Zephyr application can include them unchanged.

Planned: `include/nexus_fbl/` (memory map, image header, hand-off record) and `linker/memory_map.ld`, included by both linker scripts and by the application's build.

**The memory map is written once, in the C header** (SW-12). The C++ code includes the header; the linker script is not written by hand but produced from the header by the C preprocessor at build time, and the application includes the same header. Today `linker/memory_map.ld` still holds the numbers itself; it moves to the header in the exercise that follows SW-12.

Two rules follow from the linker reading the same macros as the compiler: the values are plain hexadecimal, with **no C suffix** (`0x00040000`, not `0x00040000u`, which `ld` rejects) and **no `ld` suffix** (not `256K`, which C rejects). The linker script keeps its `ASSERT`s: they check the layout itself (no overlap, the application inside bank 2, the version record in place), no longer a copy against another.

The flash addresses are fixed by SW-09: FBL `0x08000000` to `0x0801FFFF`, CFG and NvM after it in bank 1, the application from `0x08040000` (bank 2). The image header and the hand-off record are still to be decided and recorded as `SW-nn` in Nexus-Platform.
