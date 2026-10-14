/*
 * Flash and RAM map of the STM32F407VGT6, shared by every linker script.
 * Decision: SW-04 (flash), SW-05 (RAM). Sector layout: RM0090 Rev 22, Table 5.
 *
 * Symbols only, so each image declares its own MEMORY. Included with
 * INCLUDE memory_map.ld. The build passes -L interface/linker BEFORE -T, because ld only
 * searches the -L paths it has already seen when it reads the script.
 * interface/include/nexus_fbl/memory_map.h will carry the same numbers for C and C++
 * (not written yet). When it exists, change one, change both.
 */

/* FBL: sectors 0 to 4 (4 x 16 KB + 64 KB). The reset vector is read here. */
FBL_FLASH_ORIGIN = 0x08000000;
FBL_FLASH_LENGTH = 128K;

/*
 * Version record: 64 bytes at a fixed offset from the start of an image, after the vector table
 * (98 vectors are 392 bytes) and before the first code. Same offset in the FBL and the application,
 * so a tool reads either one the same way. Layout: version/version.hpp.in.
 */
VERSION_OFFSET = 0x200;
VERSION_SIZE   = 64;

/* Application: sectors 5 to 11 (7 x 128 KB). Starts on a 128 KB sector boundary. */
APP_FLASH_ORIGIN = 0x08020000;
APP_FLASH_LENGTH = 896K;

/* Main SRAM: SRAM1 (112 KB) and SRAM2 (16 KB), contiguous. The CCM at 0x10000000 is left empty. */
SRAM_ORIGIN = 0x20000000;
SRAM_LENGTH = 128K;

/*
 * Backup SRAM: survives a system reset (RM0090 section 7.1.3). The application writes the
 * hand-off record here before it resets, and the FBL reads it (SW-06). Both images enable
 * PWREN, PWR_CR.DBP and RCC_AHB1ENR.BKPSRAMEN before touching it.
 */
BKPSRAM_ORIGIN  = 0x40024000;
BKPSRAM_LENGTH  = 4K;

/* First 512 bytes of the backup SRAM are the hand-off record. The rest is not claimed. */
HANDOFF_ORIGIN  = BKPSRAM_ORIGIN;
HANDOFF_LENGTH  = 512;
