# Third party

Pinned external code, as git submodules. Nothing is copied in by hand. After a clone: `git submodule update --init`.

| Submodule | Pinned at | Why |
|---|---|---|
| `cmsis_core` | `v5.9.0` | Cortex-M33 core headers (`core_cm33.h`, intrinsics) |
| `cmsis_device_h5` | `v1.7.0` | STM32H533 registers (`stm32h533xx.h`) and `system_stm32h5xx.h` |

**The two move together.** ST requires a consistent CMSIS Core and Device set. The pairing for the H5 is **not yet checked** against the STM32CubeH5 release notes (the F4 pair was). Change both or neither.

CMSIS is used header-only, as SYSTEM include paths, with `-DSTM32H533xx` (`third_party/CMakeLists.txt`). `system_stm32h5xx.c` and ST's startup file are not compiled.

Still to add: a test framework (host only) and `Nexus-CAN-Utils` at a tag. How the CAN-Utils tag is taken is an open `SW` entry (phases.md).
