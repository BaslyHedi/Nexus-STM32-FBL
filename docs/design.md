# Design

Boot flow and the update state machine. Written as the design is made. Decisions that fix a value are recorded as `SW-nn` in Nexus-Platform and cited here.

## Decided

| Topic | Decision |
|---|---|
| Flash map | STM32H533RE, 512 KB in two banks, no bank swap. FBL sectors 0-15 `0x08000000` (128 KB), application sectors 16-56 `0x08020000` (328 KB), CFG sector 57 `0x08072000` (8 KB), NvM sectors 58-63 `0x08074000` (48 KB). CFG and NvM in bank 2. Details in `interface/linker/memory_map.ld`. SW-09 |
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
| RM0090, reference manual STM32F405/415, F407/417, F427/437, F429/439 | Rev 22 | `1.2-STM32F407-Discovery/` |
| DS8626, datasheet STM32F405xx, STM32F407xx | Rev 8 (DocID022152) | `1.2-STM32F407-Discovery/` |
| PM0214, STM32 Cortex-M4 MCUs and MPUs programming manual | Rev 10 | `1.2-STM32F407-Discovery/` |
| Cyphal Specification v1.0 | 2025-05-16 | `4.1-Software/` |
