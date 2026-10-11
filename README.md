# Nexus-STM32-FBL

The bare-metal flash bootloader of the STM32F407: receives an application image over Cyphal/CAN, verifies it and jumps to it (SW-01).

Part of NEXUS. The definition, decisions and phase plan live in
[Nexus-Platform](https://github.com/BaslyHedi/Nexus-Platform); this repository cites decision IDs (`HW-nn`, `SW-nn`) from there and never restates them.

**Status:** structure only, no code yet. Phase 1 of the plan is in `system/phases.md` in Nexus-Platform.

## What it implements

- Software elements: the bootloader (Nexus-Physical-01-04) and the FBL-Updater (Nexus-Physical-01-05). The trace is in [`docs/requirements.md`](docs/requirements.md).
- Decisions: SW-01 (OpenCyphal), SW-02 (in-house Cyphal stack), SW-03 (C++17 freestanding subset).

## Layout

| Folder | Owns |
|---|---|
| `interface/` | memory map, image header, hand-off record: the contract with the application |
| `cyphal/` | the in-house Cyphal stack, independent of the rest |
| `src/` | the bootloader: `boot`, `update`, `node`, `leds`, `port` |
| `drivers/stm32f407/` | register-level drivers and startup code |
| `updater/` | the FBL-Updater image |
| `linker/` | linker scripts |
| `tests/` | host tests of the bootloader logic |
| `tools/` | image tool, OpenOCD configurations |
| `third_party/` | pinned CMSIS, test framework, Nexus-CAN-Utils at a tag |
| `cmake/` | toolchain file, compile options |
| `docs/` | requirements trace, design, Cyphal scope, coding rules, LED patterns |

## Licence

Apache-2.0 ([`LICENSE`](LICENSE)). Free to use, reuse and adapt as long as you keep the [`NOTICE`](NOTICE) and credit the source.
