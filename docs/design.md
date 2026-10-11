# Design

Boot flow and the update state machine. Written as the design is made. Decisions that fix a value are recorded as `SW-nn` in Nexus-Platform and cited here.

## Decided

| Topic | Decision |
|---|---|
| Flash map | FBL sectors 0 to 4, `0x08000000` to `0x0801FFFF` (128 KB). Application sectors 5 to 11, from `0x08020000` (896 KB). SW-04 |
| RAM | Main SRAM only, CCM empty. SW-05 |
| Floating point | None in the FBL. SW-05 |
| Hand-over from the application | Backup SRAM `0x40024000`, first 512 bytes. SW-06 |
| Where the FBL runs | From the SRAM, stored in the flash. SW-07 |

## Open

How a valid application is recognised, the entry rules, the layout of the hand-off record, how this repository takes a `Nexus-CAN-Utils` tag, NvM and configuration data (Phase 2).

## Sources

Not committed. On the author's machine they are in the untracked `vendor/datasheets/` of Nexus-Platform.

| Document | Revision | Folder |
|---|---|---|
| RM0090, reference manual STM32F405/415, F407/417, F427/437, F429/439 | Rev 22 | `1.2-STM32F407-Discovery/` |
| DS8626, datasheet STM32F405xx, STM32F407xx | Rev 8 (DocID022152) | `1.2-STM32F407-Discovery/` |
| PM0214, STM32 Cortex-M4 MCUs and MPUs programming manual | Rev 10 | `1.2-STM32F407-Discovery/` |
| Cyphal Specification v1.0 | 2025-05-16 | `4.1-Software/` |
