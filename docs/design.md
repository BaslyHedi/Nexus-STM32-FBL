# Design

Boot flow and the update state machine. Written as the design is made. Decisions that fix a value are recorded as `SW-nn` in Nexus-Platform and cited here.

## Decided

| Topic | Decision |
|---|---|
| Flash map | FBL sectors 0 to 4, `0x08000000` to `0x0801FFFF` (128 KB). Application sectors 5 to 11, from `0x08020000` (896 KB). SW-04 |
| RAM | Main SRAM only, CCM empty. SW-05 |
| Floating point | None in the FBL. SW-05 |

## Open

How a valid application is recognised, the entry rules, the hand-off from the application and where its block sits in the SRAM, how this repository takes a `Nexus-CAN-Utils` tag, NvM and configuration data (Phase 2).
