# Interface

The contract between the bootloader and what it loads, and between this repository and its consumers. Plain C headers (`.h`), so the Zephyr application can include them unchanged.

Planned: `include/nexus_fbl/` (memory map, image header, hand-off record) and `linker/memory_map.ld`, included by both linker scripts and by the application's build.

The values are decided when the STM32 work starts and recorded as `SW-nn` in Nexus-Platform. Nothing is written here before that.
