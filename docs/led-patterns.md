# LED patterns

One pattern per state, distinct at arm's length (T1.3). Required by Nexus-Physical-01-04-R010, Nexus-Physical-01-04-R011, Nexus-Physical-01-05-R004 and, in the application repository, Nexus-Physical-01-06-R005.

The bootloader flashes fast and the application flashes slowly, so the speed alone tells which one is running.

| State | Pattern |
|---|---|
| FBL idle | Burst chase, below |
| FBL receiving | to define |
| FBL-Updater running | to define |
| Application running | defined in `Nexus-STM32-App` |

## The LEDs

Discovery board user LEDs, active high. Seen from above, PD12 to PD15 go round clockwise.

| Pin | LED | Colour | Position |
|---|---|---|---|
| PD12 | LD4 | green | left |
| PD13 | LD3 | orange | top |
| PD14 | LD5 | red | right |
| PD15 | LD6 | blue | bottom |

## FBL idle: burst chase

One LED at a time. It flashes six times, then the next LED takes over, clockwise: green, orange, red, blue, then green again. The others stay off.

One LED's burst:

| Step | LED | Time |
|---|---|---|
| 1 | on | 70 ms |
| 2 | off | 30 ms |
| 3 | on | 70 ms |
| 4 | off | 30 ms |
| 5 | on | 70 ms |
| 6 | off | 30 ms |
| 7 | on | 70 ms |
| 8 | off | 30 ms |
| 9 | on | 70 ms |
| 10 | off | 30 ms |
| 11 | on | 70 ms |
| 12 | off | 230 ms |

A burst lasts 800 ms, a full turn of the four LEDs 3200 ms. The table is `idleSteps` in `src/leds/patterns.hpp`, row for row; `static_assert`s there check the number of steps and the 800 ms.
