# Coding rules

The subset of C++17 the FBL uses (SW-03).

- Standard C++17, `arm-none-eabi-gcc`.
- No exceptions, no RTTI, no dynamic allocation.
- No standard library that allocates or pulls in I/O.
- Reference rule set: MISRA C++:2023. Deviations are listed here with a reason.
- Allowed standard headers, listed as they are needed: `<cstddef>`, `<cstdint>`.
- Stand-ins for C++20 (span, byte order and bit helpers, `static_assert` for driver contracts) are kept in one place each, so the later migration finds them.

## Deviations

| Where | Rule | Reason |
|---|---|---|
| `drivers/stm32f407/startup/startup.cpp` | no function-like macros | one list of the 82 interrupt names declares the weak handlers and fills the vector table. Written twice by hand, the two would drift. |
| `drivers/stm32f407/startup/startup.cpp` | reserved identifiers (`__stack_top`, ...) | the names are the linker script's symbols (`linker/fbl.ld`), shared with the script. |
| `drivers/stm32f407/startup/startup.cpp` | `main` is called | the start-up code has to; it goes through an asm label (`callMain`). |
