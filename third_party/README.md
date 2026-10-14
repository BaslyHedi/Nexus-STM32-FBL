# Third party

Pinned external code, as git submodules. Nothing is copied in by hand. After a clone: `git submodule update --init`.

| Submodule | Pinned at | Why |
|---|---|---|
| `cmsis_core` | `v5.9.0` | Cortex-M4 core headers (`core_cm4.h`, intrinsics) |
| `cmsis_device_f4` | `v2.6.11` | STM32F407 registers (`stm32f407xx.h`) and `system_stm32f4xx.h` |

**The two move together.** ST requires a consistent CMSIS Core and Device set. This pair is the one listed in the STM32CubeF4 v1.28.3 release notes. Change both or neither.

CMSIS is used header-only, as SYSTEM include paths, with `-DSTM32F407xx` (`third_party/CMakeLists.txt`). `system_stm32f4xx.c` and ST's startup file are not compiled.

Still to add: a test framework (host only) and `Nexus-CAN-Utils` at a tag. How the CAN-Utils tag is taken is an open `SW` entry (phases.md).
