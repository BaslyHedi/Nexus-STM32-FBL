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

One LED at a time. It flashes four times, then the next LED takes over, clockwise: green, orange, red, blue, then green again. The others stay off.

One LED's burst:

| Step | LED | Time |
|---|---|---|
| 1 | on | 100 ms |
| 2 | off | 50 ms |
| 3 | on | 100 ms |
| 4 | off | 50 ms |
| 5 | on | 100 ms |
| 6 | off | 50 ms |
| 7 | on | 100 ms |
| 8 | off | 50 ms |
| 9 | off | 500 ms |

A burst lasts 1100 ms, a full turn of the four LEDs 4400 ms.
