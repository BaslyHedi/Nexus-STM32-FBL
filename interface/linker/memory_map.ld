/*
 * Flash and RAM map of the STM32F407VGT6, shared by every linker script.
 * Decision: SW-04 (flash), SW-05 (RAM). Sector layout: RM0090 Rev 22, Table 5.
 *
 * Symbols only, so each image declares its own MEMORY. Included with
 * INCLUDE memory_map.ld. The build passes -L interface/linker BEFORE -T, because ld only
 * searches the -L paths it has already seen when it reads the script.
 * interface/include/nexus_fbl/memory_map.h carries the same numbers for C and C++.
 * Change one, change both.
 */

/* FBL: sectors 0 to 4 (4 x 16 KB + 64 KB). The reset vector is read here. */
FBL_FLASH_ORIGIN = 0x08000000;
FBL_FLASH_LENGTH = 128K;

/* Application: sectors 5 to 11 (7 x 128 KB). Starts on a 128 KB sector boundary. */
APP_FLASH_ORIGIN = 0x08020000;
APP_FLASH_LENGTH = 896K;

/* Main SRAM: SRAM1 (112 KB) and SRAM2 (16 KB), contiguous. The CCM at 0x10000000 is left empty. */
SRAM_ORIGIN = 0x20000000;
SRAM_LENGTH = 128K;
