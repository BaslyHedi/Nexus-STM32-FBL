# LEDs

The three LED patterns of the bootloader (T1.3): FBL idle, FBL receiving, FBL-Updater running. The fourth state, the application, has its pattern in `Nexus-STM32-App`. The patterns themselves are written in [`../../docs/led-patterns.md`](../../docs/led-patterns.md).

- `patterns.hpp`: each pattern as a `constexpr` table of steps, one row per row of the doc, with `static_assert`s on the step count and the duration. A step holds a frame: one 4-bit level per LED of the stick, LED 1 in bits 0 to 3. LD2 shows LED 1. A state has a pattern for LD2 and one for the stick, sharing a table where they show the same thing. The chase rotation is `constexpr` and checked there too.
- `player.hpp`, `player.cpp`: turns a millisecond clock into the frame due. It does not drive the LEDs, the caller writes the frame, so it has no hardware dependency and a host test can drive it with any sequence of times. One player per output, started on the same tick.

A pattern needs a pass duration above zero: the player walks through steps in a loop, and a pattern of 0 ms steps would never leave it. The `static_assert` on the duration rules that out.
