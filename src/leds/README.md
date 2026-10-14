# LEDs

The three LED patterns of the bootloader (T1.3): FBL idle, FBL receiving, FBL-Updater running. The fourth state, the application, has its pattern in `Nexus-STM32-App`. The patterns themselves are written in [`../../docs/led-patterns.md`](../../docs/led-patterns.md).

- `patterns.hpp`: each pattern as a `constexpr` table of steps, one row per row of the doc, with `static_assert`s on the step count and the duration.
- `player.hpp`, `player.cpp`: turns a millisecond clock into the mask due. It does not drive the LEDs, the caller writes the mask, so it has no hardware dependency and a host test can drive it with any sequence of times.

A pattern needs a pass duration above zero: the player walks through steps in a loop, and a pattern of 0 ms steps would never leave it. The `static_assert` on the duration rules that out.
