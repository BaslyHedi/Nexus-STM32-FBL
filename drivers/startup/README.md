# Startup

`startup.cpp`: the vector table, `Default_Handler`, `Reset_Handler` and the reset sequence (SW-03, SW-07).

## What is in it

- **`kVectorTable`**, 149 words in `.isr_vector`: the initial stack pointer (`__stack_top`), `Reset_Handler`, the system handlers and the 133 interrupt vectors (`ReservedN` where the header leaves a number out), in the order of RM0481 Table 153. A handler nobody defines is the weak alias of `Default_Handler`, a loop in the SRAM. NMI and the five fault handlers (with SecureFault) are the exception: they alias `Fault_Handler`, a loop in `.boot_text`, because until the copy is done the table in the flash is the live one and a handler in the SRAM would be unprogrammed memory. A driver defines `extern "C" void CAN1_RX0_IRQHandler()` and it replaces the alias.
- **`Reset_Handler`**, naked: loads `MSP` and calls `Reset_Main`. The core has loaded `MSP` from word 0 already; doing it again lets a debugger that only sets the PC start the same way.
- **`Reset_Main`**, in `.boot_text`, in this order: copy `.ram_image`, copy the vector table, write `SCB->VTOR` after the copy is complete (Armv8-M, `VTOR`), clear `.bss`, run `.preinit_array` and `.init_array`, call `main`.

## Why ST's file is not used

`third_party/cmsis_device_h5/Source/Templates/gcc/startup_stm32h533xx.s` copies `.data` only, and calls `SystemInit` and `main` from the flash. The FBL copies its code too and runs from the SRAM. The file was the reference for the vector order. `system_stm32h5xx.c` is not compiled either: its `SystemInit` would be code in `.ram_image`.

## Rules for this file

- `Reset_Main` and everything it calls before the copy must be in `.boot_text` or inlined. A helper outside it lands in `.ram_image`, which is not copied yet. The loops are `always_inline` and the file is built with `-fno-tree-loop-distribute-patterns`, so GCC does not turn them into `memcpy` and `memset` calls.
- The call to `main` crosses from the flash into the SRAM, further than a `bl` reaches. The linker adds a veneer (`__main_veneer`); check in the disassembly that it is in `.boot_text`:
  `arm-none-eabi-objdump -d -j .boot_text build/output/FBL_Nexus.elf`, and the same for `FBL_Nexus_debug.elf`.
  The debug launch stops the core at `__main_veneer`, after the copy, before it sets any breakpoint in the SRAM (`tools/README.md`). A change that removes or renames the veneer breaks the launch.
- `.init_array` is expected to be empty. The build fails if it is not (`cmake/check-init-array.cmake`).
