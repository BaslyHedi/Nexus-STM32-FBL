/*
 * Flash and RAM map of the STM32H533RE, shared by every linker script.
 * Decisions: SW-04 (flash), SW-05 (RAM), SW-06 (hand-off), HW-22 (the part). Sector layout: RM0481
 * Rev 5, Table 53. Reworked for the H533 from the F407 map.
 *
 * Symbols only, so each image declares its own MEMORY. Included with
 * INCLUDE memory_map.ld. The build passes -L interface/linker BEFORE -T, because ld only
 * searches the -L paths it has already seen when it reads the script.
 * interface/include/nexus_fbl/memory_map.h will carry the same numbers for C and C++
 * (not written yet). When it exists, change one, change both.
 *
 * Flash: 512 KB in two banks of 32 sectors of 8 KB, contiguous from 0x08000000. No bank swap
 * (SWAP_BANK is not used): one image, one start address, the FBL flashes the application in place.
 * A read from a bank stalls while that bank is written or erased (RM0481 section 7.3.7), and
 * only one write or erase runs at a time on the whole flash (one BSY flag).
 *
 *   sectors   0-15  0x08000000  128 KB  FBL            bank 1
 *   sectors  16-56  0x08020000  328 KB  application    bank 1 and bank 2 (bank 2 starts at 0x08040000)
 *   sector      57  0x08072000    8 KB  CFG            bank 2, configuration that never changes at run time
 *   sectors  58-63  0x08074000   48 KB  NvM            bank 2, data that changes at run time
 *
 * The CFG and NvM regions are shared between the FBL and the application. Their content and
 * interface are not defined yet.
 */

/* FBL: sectors 0 to 15. The reset vector is read here (the default boot address, NSBOOTADD). */
FBL_FLASH_ORIGIN = 0x08000000;
FBL_FLASH_LENGTH = 128K;

/*
 * Version record: 64 bytes at a fixed offset from the start of an image, after the vector table
 * and before the first code. The H533 table has 149 vectors (16 + 133 interrupts, RM0481 Table
 * 153), 596 bytes, so the record is at 0x400, the next 1 KB boundary and the VTOR alignment
 * (_Vector_Alignment in fbl.ld). Was 0x200 on the F407 (SW-08). Layout: version/version.hpp.in.
 */
VERSION_OFFSET = 0x400;
VERSION_SIZE   = 64;

/* Application: sectors 16 to 56. Starts on a sector boundary, at the address the F407 map used. */
APP_FLASH_ORIGIN = 0x08020000;
APP_FLASH_LENGTH = 328K;

/* CFG: sector 57. Written rarely, read by both images. */
CFG_FLASH_ORIGIN = 0x08072000;
CFG_FLASH_LENGTH = 8K;

/* NvM: sectors 58 to 63, the end of bank 2. */
NVM_FLASH_ORIGIN = 0x08074000;
NVM_FLASH_LENGTH = 48K;

/*
 * Main SRAM: SRAM1 (128 KB). SRAM2 (80 KB, ECC) and SRAM3 (64 KB) follow it and are left to the
 * application. An unwritten word of an ECC SRAM can raise an error when it is read.
 */
SRAM_ORIGIN = 0x20000000;
SRAM_LENGTH = 128K;

/*
 * Backup SRAM: 2 KB with ECC, survives a system reset (RM0481 Table 43, section 2.3.3). The
 * application writes the hand-off record here before it resets, and the FBL reads it (SW-06).
 * Both images enable the BKPSRAM clock and the backup domain write access before touching it.
 * Non-secure address, TrustZone off (TZEN = 0xC3).
 */
BKPSRAM_ORIGIN  = 0x40036400;
BKPSRAM_LENGTH  = 2K;

/* First 512 bytes of the backup SRAM are the hand-off record. The rest is not claimed. */
HANDOFF_ORIGIN  = BKPSRAM_ORIGIN;
HANDOFF_LENGTH  = 512;
