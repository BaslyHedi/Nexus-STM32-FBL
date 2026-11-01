# LED patterns

One pattern per state, distinct at arm's length (T1.3). Required by Nexus-Physical-01-04-R010, Nexus-Physical-01-04-R011, Nexus-Physical-01-05-R004 and, in the application repository, Nexus-Physical-01-06-R005.

The bootloader flashes fast and the application flashes slowly, so the speed alone tells which one is running.

| State | Pattern |
|---|---|
| FBL idle | Burst chase, below |
| FBL receiving | Circle, below |
| FBL-Updater running | All flash, below |
| Application running | defined in `Nexus-STM32-App` |

## The LEDs

The four-LED design below was made for the STM32F4DISCOVERY's user LEDs, active high, PD12 to PD15 going round clockwise. The NUCLEO-H533RE has one user LED, LD2 on PA5 (HW-22): `Leds::write` drives it from bit 0 (green) and drops the rest. The patterns are redesigned for the LED strip component, which comes later.

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

## FBL receiving: circle

One LED at a time, 100 ms each, clockwise without a pause: green, orange, red, blue, then green again. A full turn lasts 400 ms.

| Step | LED | Time |
|---|---|---|
| 1 | on | 100 ms |

The table is `receivingSteps` in `src/leds/patterns.hpp`: one step on the first LED, moved one LED clockwise on each pass.

## FBL-Updater running: all flash

The four LEDs together, 100 ms on, 100 ms off: five flashes a second.

| Step | LEDs | Time |
|---|---|---|
| 1 | all on | 100 ms |
| 2 | all off | 100 ms |

The table is `updaterSteps` in `src/leds/patterns.hpp`.
