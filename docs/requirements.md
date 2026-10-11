# Requirements trace

This repository implements two software elements of Nexus-Platform: the bootloader (01-04) and the FBL-Updater (01-05), see the configuration table in `requirements/system/Nexus-Enabling.md`. The Platform states the requirements on the elements. This file cites their IDs and holds the software requirements derived from them (Nexus-Enabling-R003).

Derived requirements are written here by the author as the design is made. Each cites its parent. The `FBL-SWR` column stays empty until then.

## Bootloader, Nexus-Physical-01-04

| Parent | Topic | Verification | FBL-SWR |
|---|---|---|---|
| 01-04-R001 | Operate CAN1 at the matrix bit rate | T1.1 | |
| 01-04-R002 | Pull the image by Cyphal file transfer on an update request | T1.1 | |
| 01-04-R003 | Erase and program the application region | T1.1 | |
| 01-04-R004 | Compute the CRC of the programmed image | T1.1 | |
| 01-04-R005 | Jump to the application when the CRC matches | T1.1 | |
| 01-04-R006 | Stay reachable after an interrupted download | T1.1 | |
| 01-04-R007 | Stay reachable when there is no valid image | T1.1 | |
| 01-04-R008 | Reside in the bootloader region of the memory map | Inspection | |
| 01-04-R009 | Download and start the FBL-Updater as an application image | T1.2 | |
| 01-04-R010 | Idle pattern on the four LEDs | T1.3 | |
| 01-04-R011 | Receiving pattern on the four LEDs | T1.3 | |
| 01-04-R012 | Report the bootloader version in the node info | T1.1 | |

## FBL-Updater, Nexus-Physical-01-05

| Parent | Topic | Verification | FBL-SWR |
|---|---|---|---|
| 01-05-R001 | Be an image for the application region | T1.2 | |
| 01-05-R002 | Rewrite the bootloader region with a new bootloader | T1.2 | |
| 01-05-R003 | Verify the rewritten bootloader region | T1.2 | |
| 01-05-R004 | FBL-Updater pattern on the four LEDs | T1.3 | |
| 01-05-R005 | Report the FBL-Updater version in the node info | T1.2 | |

IDs are written short here. The full form is `Nexus-Physical-01-04-R001`. The statements are in Nexus-Platform and are not restated.

## Decisions implemented

SW-01 (OpenCyphal), SW-02 (in-house Cyphal stack), SW-03 (C++17 freestanding subset), SW-04 (flash map), SW-05 (SRAM, CCM and FPU), SW-06 (hand-off through the backup SRAM), SW-07 (FBL runs from the SRAM).
