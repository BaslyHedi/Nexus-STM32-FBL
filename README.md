# Nexus-STM32-FBL

The bare-metal flash bootloader of the STM32F407: receives an application image over Cyphal/CAN, verifies it and jumps to it (SW-01).

Part of NEXUS. The definition, decisions and phase plan live in
[Nexus-Platform](https://github.com/BaslyHedi/Nexus-Platform); this repository cites decision IDs (`HW-nn`, `SW-nn`) from there and never restates them.

**Status:** created 2026-10-11, empty on purpose. Phase 1 of the plan is in `system/phases.md` in Nexus-Platform.
