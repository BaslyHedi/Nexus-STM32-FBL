# Version

The version record of the image: 64 bytes at a fixed address, readable with `strings` on the `.elf`, or at `0x08000200` in a hex or SREC dump.

| Offset | Size | Content | Now |
|---|---|---|---|
| 0 | 3 | software type | `FBL` (`APP` for the application) |
| 3 | 1 | separator | `_` |
| 4 | 5 | platform name | `Nexus` |
| 9 | 1 | separator | `_` |
| 10 | 2 | platform version | `v1` |
| 12 | 1 | separator | `_` |
| 13 | 8 | software version `AA-BB-CC`, major, minor, bugfix | `00-01-00` |
| 21 | 1 | NUL | |
| 22 | 42 | reserved, `0xFF` | |

`FBL_Nexus_v1_00-01-00`, then the NUL, then `0xFF`. The reserved bytes are the erased state of the flash, so a later field can be programmed into them without an erase.

## Files

| File | Content |
|---|---|
| `version.hpp.in` | template; CMake generates `build/generated/version/version.hpp` (not committed) with the strings, the numbers and the `Record` struct |
| `version.cpp` | builds the one `kRecord` at compile time and puts it in the section `.version` |

The values come from `cmake/project-settings.cmake`; the field widths are written once, in `cmake/version.cmake`, and `version.hpp.in` takes them from there. `cmake/version.cmake` fails the configure when a value does not fit the layout: a version part above 99, a platform name that is not 5 characters, a platform version that is not 2.

## Where it sits

`interface/linker/memory_map.ld` sets `VERSION_OFFSET = 0x200` and `VERSION_SIZE = 64`. `linker/fbl.ld` places `.version` there, after the vector table (392 bytes) and before `.boot_text`, and fails the link if the record is missing, not 64 bytes or not at that address. The gap between the table and the record is never programmed. The offset is the same for the application (`0x08020200`), so a tool reads either image the same way; the application's linker script has to leave the slot free (SW-08; the application side is still to do).

The record is not copied to the SRAM and the FBL does not read it. The layout is a contract with the application and the image tool, decided in SW-08. The application follows the same pattern later, on its own side.
