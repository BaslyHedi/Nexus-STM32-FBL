# LED patterns

One pattern per state, distinct at arm's length (T1.3), on the Nucleo's user LED and on the LED stick. Required by Nexus-Physical-01-04-R010, Nexus-Physical-01-04-R011, Nexus-Physical-01-05-R004 and, in the application repository, Nexus-Physical-01-06-R005. The stick itself is Nexus-Physical-01-07 (BOM 1.3).

The bootloader flashes fast and the application moves slowly, so the speed alone tells which one is running.

## How the four states differ

Every state has a colour, a motion and a speed. Any two states differ in at least two of the three, so a glance, a colour-blind viewer or a stick seen at an angle still tells them apart.

| State | Colour | Motion on the stick | Speed | LD2 |
|---|---|---|---|---|
| FBL idle | blue | one LED, in bursts, stepping along | fast flicker, slow travel | the burst |
| FBL receiving | amber | a comet travelling, over a progress fill | 100 ms per LED | 100 ms on, 300 ms off |
| FBL-Updater running | magenta | all eight flash in place | 5 flashes a second | the same flash |
| Application running | green, proposed | all eight breathe | 2 s | defined in `Nexus-STM32-App` |

Blue is maintenance, amber is busy, magenta warns that the FBL itself is being rewritten (do not cut the power), green is healthy. **Red is kept free** for a fault state later.

## The LEDs

| LED | Pin | What it is |
|---|---|---|
| LD2 | PA5 | the Nucleo's green user LED, on or off (HW-22) |
| Stick, LEDs 1 to 8 | PC12, SPI3_MOSI | eight WS2812B, chained on one data line (HW-22, HW-24) |

**LED 1 is the first on the chain**, at DI. "Moves on" means from LED 1 towards LED 8, and from LED 8 back to LED 1. Whether LED 1 is on the viewer's left is checked on the vehicle; if it is not, only the order of the LEDs is reversed, not the tables.

## Frames and levels

A step of a pattern holds a **frame**: a `uint32_t` with one 4-bit level per stick LED. LED 1 is bits 0 to 3, LED 8 is bits 28 to 31. Level 0 is off, level 15 is the full colour of the pattern.

| Frame | Meaning |
|---|---|
| `0x0000000F` | LED 1 at level 15, the others off |
| `0x6200000F` | LED 1 at 15, LED 8 at 6, LED 7 at 2 |
| `0xFFFFFFFF` | all eight at 15 |

**LD2** uses the same frames: it is on when the level of LED 1 is above 0. So LD2 and the stick can share a table where they show the same thing.

**Motion.** *Still*: the steps play as written. *Chase*: at the end of each pass every level moves on one LED, a rotation of the frame by 4 bits towards LED 8, LED 8 wrapping to LED 1. Eight passes bring the frame back to the start. A chase on LD2 is never used: LD2 would show only one pass in eight.

**Colour.** A pattern of the stick has one colour, given at level 15. The level scales it linearly, each channel being `colour * level / 15`, rounded down. The bench may replace the linear scale by a table later, if the low levels look uneven.

| Colour | R | G | B |
|---|---|---|---|
| blue | 0 | 0 | 40 |
| amber | 40 | 15 | 0 |
| magenta | 40 | 0 | 30 |
| green | 0 | 40 | 0 |

The values are set at about a sixth of the LED's range: bright enough at arm's length, not blinding, and the worst case (all eight magenta) draws about 50 mA instead of the stick's 480 mA. The bench sets the final values. On the wire the order is **green, red, blue** (the WS2812B's), the tables stay in R, G, B.

## FBL idle: burst chase

**Stick:** one blue LED plays the burst below, then the next LED takes over. A burst lasts 800 ms, a sweep of the eight LEDs 6.4 s. **LD2** plays the same burst, still, so the board and the stick flash together.

| Step | Frame | Time |
|---|---|---|
| 1 | `0x0000000F` | 70 ms |
| 2 | `0x00000000` | 30 ms |
| 3 | `0x0000000F` | 70 ms |
| 4 | `0x00000000` | 30 ms |
| 5 | `0x0000000F` | 70 ms |
| 6 | `0x00000000` | 30 ms |
| 7 | `0x0000000F` | 70 ms |
| 8 | `0x00000000` | 30 ms |
| 9 | `0x0000000F` | 70 ms |
| 10 | `0x00000000` | 30 ms |
| 11 | `0x0000000F` | 70 ms |
| 12 | `0x00000000` | 230 ms |

| | Table | Motion | Colour |
|---|---|---|---|
| Stick | the twelve steps above | chase | blue |
| LD2 | the same twelve steps | still | |

## FBL receiving: comet over a progress fill

**Stick:** an amber comet, its head at level 15 and its tail at 6 and 2, moves on one LED every 100 ms; a sweep lasts 800 ms. Under it the stick fills up with the progress of the download, below. **LD2:** 100 ms on, 300 ms off, as on the four-LED design.

| | Step | Frame | Time | Motion | Colour |
|---|---|---|---|---|---|
| Stick | 1 | `0x6200000F` | 100 ms | chase | amber |
| LD2 | 1 | `0x0000000F` | 100 ms | still | |
| LD2 | 2 | `0x00000000` | 300 ms | still | |

### The progress fill

The update logic reports the bytes received and the size of the image, from `uavcan.file.GetInfo` ([cyphal-scope.md](cyphal-scope.md)). The LED side turns them into a fill from 0 to 32, four steps per LED:

    fill = received * 32 / size, rounded down

LED *i* (1 to 8) gets the fill level `fill - 4 * (i - 1)`, held between 0 and 4. A full LED is at level 4, the one being filled steps through 1, 2, 3. 32 steps, about 3 % each; the fill shows 32 only when every byte is in.

**The comet and the fill combine LED by LED, the higher level wins.** The comet's head (15) and the first LED of its tail (6) stay visible over the fill, the last one (2) disappears into it. At 100 % the stick is a steady amber bar with the head still running over it, which shows the FBL is alive while it verifies.

| Case | The stick |
|---|---|
| The size is not known (GetInfo unanswered or failed) | the comet alone, no fill |
| More bytes received than the size | the fill held at 32 (the update logic aborts, [cyphal-scope.md](cyphal-scope.md)) |

**A stalled transfer** shows as a fill that stops growing while the comet keeps running: the CAN side is stuck, not the MCU.

## FBL-Updater running: all flash

All eight LEDs together, 100 ms on, 100 ms off: five flashes a second, magenta. LD2 flashes with them.

| Step | Frame | Time |
|---|---|---|
| 1 | `0xFFFFFFFF` | 100 ms |
| 2 | `0x00000000` | 100 ms |

| | Table | Motion | Colour |
|---|---|---|---|
| Stick | the two steps above | still | magenta |
| LD2 | the same two steps | still | |

## Application running: proposed to `Nexus-STM32-App`

All eight LEDs green, breathing: the level rises from 0 to 15 and falls back in 2 s, smoothly. It needs many frames a cycle, which is the opposite of the FBL's hard steps. The application repository owns it and may change it, as long as it stays green and slow.

## The pipeline

Each stage is a plain function or object, testable on the host, except the driver.

    player of the stick (time) -> frame -+
                                         +-> combine -> frame -> colour -> 8 x (G, R, B) -> driver, SPI3 on PC12
    progress (bytes) ------> fill 0..32 -+

    player of LD2 (time) ----> frame -> LED 1 above 0 -> LD2 on PA5

- **Who chooses the pattern.** The loop in `main` reads the state and the progress from the update logic and passes them on; the update logic does not know the LEDs and the LEDs do not know the update logic. A state change starts both players on the same tick. Starting them again on every pass would freeze them on their first step.
- **Only a change is sent.** The combined frame (one `uint32_t`) is compared with the last one sent; the colour and the encoding run only when it differs. A frame on the wire takes about 240 µs.

## The stick on the wire

From the stick's datasheet summary in Nexus-Platform (`bom/parts/1.3-WS2812B-LED-Stick/SPECS.md`). The driver design is for the driver exercise; these are the constraints it has to meet.

- **24 bits per LED, green, red, blue, most significant bit first**, LED 1 first. 192 bits for the stick.
- **One bit lasts 1.25 µs.** A 0 is high 0.40 µs then low 0.85 µs, a 1 is high 0.80 µs then low 0.45 µs, each phase within about 150 ns. A low phase that runs long is tolerated, up to the latch time.
- **A low of 300 µs or more latches the frame.** The line must idle low between frames.
- **The bits come from SPI3**, each LED bit sent as a few SPI bits on MOSI. The SPI clock and the number of SPI bits per LED bit are chosen so that both phases of a 0 and of a 1 fall in the limits above.
- **The stick keeps its last frame through an MCU reset**, as long as it has 5 V. The FBL sends its first frame early in its start-up, and blanks the stick before it jumps to the application.
- **3.3 V on DI is below the WS2812B's 3.5 V minimum at 5 V.** If LED 1 flickers or shows the wrong colour, the fallback is PC12 open-drain with a 1 kΩ pull-up to 5 V (PC12 is 5 V tolerant).
