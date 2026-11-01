# Design

Boot flow and the update state machine. Written as the design is made. Decisions that fix a value are recorded as `SW-nn` in Nexus-Platform and cited here.

## Decided

| Topic | Decision |
|---|---|
| Flash map | STM32H533RE, 512 KB in two banks, no bank swap. Bank 1: FBL sectors 0-15 `0x08000000` (128 KB), CFG sector 16 `0x08020000` (8 KB), NvM sectors 17-22 `0x08022000` (48 KB), reserve sectors 23-31 `0x0802E000` (72 KB). Bank 2: application sectors 32-63 `0x08040000` (256 KB), all of it. Details in `interface/linker/memory_map.ld`. SW-09 |
| RAM | SRAM1 128 KB at `0x20000000`. SRAM2 and SRAM3 left to the application. SW-10 |
| Floating point | None in the FBL. SW-05 |
| Hand-over from the application | Backup SRAM `0x40036400`, first 512 bytes of 2 KB. SW-10 |
| Where the FBL runs | From the SRAM, stored in the flash. SW-07, SW-11 |
| Version record | At offset `0x400`: the 149-vector table is 596 bytes. SW-11 |

## Open

How a valid application is recognised, the entry rules, the layout of the hand-off record, how this repository takes a `Nexus-CAN-Utils` tag, the content and interface of CFG and NvM (shared with the application).

## Sources

Not committed. On the author's machine they are in the untracked `vendor/datasheets/` of Nexus-Platform.

| Document | Revision | Folder |
|---|---|---|
| RM0481, reference manual STM32H523/533, H562/563/573, H543/553 | Rev 5 | `1.2-NUCLEO-H533RE/` |
| DS14539, datasheet STM32H533xx | Rev 3 | `1.2-NUCLEO-H533RE/` |
| UM3121, STM32H5 Nucleo-64 boards (MB1814) | | `1.2-NUCLEO-H533RE/` |
| Cyphal Specification v1.0 | 2025-05-16 | `4.1-Software/` |
