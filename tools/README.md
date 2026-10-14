# Tools

Host-side scripts: the image tool that adds the header and CRC, OpenOCD configurations for the ST-LINK recovery path.

## Flash and debug

Through the ST-LINK/V2 on the STM32F4DISCOVERY, with OpenOCD (tested with 0.12.0) and `gdb-multiarch`. In VS Code, with the extensions of `.vscode/extensions.json` (Cortex-Debug for the debugger).

| File | Use |
|---|---|
| `openocd/fbl.cfg` | ST-LINK, SWD, the F4 target, NRST. Connects to a running core without resetting it. |
| `openocd/connect-under-reset.cfg` | added after `fbl.cfg` to flash and to launch a debug session: holds NRST while connecting |

**Tasks** (`Terminal > Run Task`): `FBL: build` (the default build task, both variants), `FBL: flash release (SREC)`, `FBL: flash debug (ELF)`, `FBL: read version record`. A flash builds first, erases only the sectors the image covers (SW-04), verifies and resets.

**Debug** (`Run and Debug`): `flash and debug` builds, flashes the variant's `.elf` and stops in `main`; `attach` stops the running core with no load and no reset, and the image on the chip has to be the variant chosen. The debug variant (`-Og`) steps line by line; the release one (`-Os`) is the image that ships.

On the command line:

```
openocd -f tools/openocd/fbl.cfg -f tools/openocd/connect-under-reset.cfg \
        -c "program build/output/FBL_Nexus.srec verify reset exit"
```

### Things to know

- **Connect under reset to flash.** The image found on a new board slept or took the SWD pins, and the plain connection failed. Holding NRST works whatever the image does. OpenOCD keeps NRST low after that connection until a reset command, so Attach must not use that file: it would find the core in reset.
- **A breakpoint in the SRAM code is set after the start-up copy.** The FBL runs from the SRAM (SW-07). The Cortex-M4 breakpoint unit only reaches addresses below `0x20000000`, so a breakpoint in `.ram_image` is a `BKPT` instruction that GDB writes into the SRAM, and the start-up copy would overwrite one written before it. `launch.json` therefore runs the start-up code to `__main_veneer` (a hardware breakpoint, in the flash, after the copy) before the breakpoints of the editor are set, at the launch and at every restart or reset. Breakpoints in `Reset_Handler`, `Reset_Main` and anything else in `.boot_text` are hardware ones and work from the first instruction.
- **`__main_veneer` is made by the linker** for the call from the flash to `main` in the SRAM, too far for a `bl` (`drivers/startup/startup.cpp`). If it ever disappears, the launch fails at the `eval` line: run to another address in `Reset_Main` after the copy.
- **No register view yet.** Cortex-Debug shows the peripherals from an SVD file, which is not in `vendor/` (`svdFile` in `launch.json`).
