# Interface

The contract between the bootloader and what it loads, and between this repository and its consumers. Plain C headers (`.h`), so the Zephyr application can include them unchanged.

Planned: `include/nexus_fbl/` (memory map, image header, hand-off record) and `linker/memory_map.ld`, included by both linker scripts and by the application's build.

The flash addresses are fixed by SW-09: FBL `0x08000000` to `0x0801FFFF`, CFG and NvM after it in bank 1, the application from `0x08040000` (bank 2). The image header and the hand-off record are still to be decided and recorded as `SW-nn` in Nexus-Platform.
