# Nexus-STM32-FBL

The bare-metal flash bootloader of the STM32H533: receives an application image over Cyphal/CAN, verifies it and jumps to it (SW-01).

Part of NEXUS. The definition, decisions and phase plan live in
[Nexus-Platform](https://github.com/BaslyHedi/Nexus-Platform); this repository cites decision IDs (`HW-nn`, `SW-nn`) from there and never restates them.

**Status:** the build, the start-up code and the version record exist, and the image is flashed and debugged from VS Code through the ST-LINK. The LEDs show the FBL patterns. The Cyphal stack has its first layer, the transfer CRC ([`cyphal/README.md`](cyphal/README.md)); the bootloader logic does not exist yet. Build: [`cmake/README.md`](cmake/README.md). Flash and debug: [`tools/README.md`](tools/README.md). Phase 1 of the plan is in `system/phases.md` in Nexus-Platform.

## What it implements

- Software elements: the bootloader (Nexus-Physical-01-04) and the FBL-Updater (Nexus-Physical-01-05). The trace is in [`docs/requirements.md`](docs/requirements.md).
- Decisions: SW-01 (OpenCyphal), SW-02 (in-house Cyphal stack), SW-03 (C++17 freestanding subset), SW-04 (flash map), SW-05 (SRAM, CCM and FPU), SW-06 (hand-off through the backup SRAM), SW-07 (FBL runs from the SRAM), SW-08 (version record).

## Layout

| Folder | Owns |
|---|---|
| `interface/` | memory map, image header, hand-off record: the contract with the application |
| `cyphal/` | the in-house Cyphal stack, independent of the rest |
| `src/` | the bootloader: `boot`, `update`, `node`, `leds`, `port` |
| `drivers/` | `startup` (vector table, reset sequence), `runtime` (the memory functions GCC calls), `stm32h533` (register-level drivers) |
| `updater/` | the FBL-Updater image |
| `linker/` | linker scripts |
| `tests/` | host tests of the bootloader logic |
| `tools/` | image tool, OpenOCD configurations |
| `version/` | the version record placed in the image |
| `third_party/` | pinned CMSIS, test framework, Nexus-CAN-Utils at a tag |
| `cmake/` | toolchain file, compile options, project settings, post-build steps |
| `docs/` | requirements trace, design, Cyphal scope, LED patterns |

## Licence

Apache-2.0 ([`LICENSE`](LICENSE)). Free to use, reuse and adapt as long as you keep the [`NOTICE`](NOTICE) and credit the source.
