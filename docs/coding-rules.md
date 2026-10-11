# Coding rules

The subset of C++17 the FBL uses (SW-03).

- Standard C++17, `arm-none-eabi-gcc`.
- No exceptions, no RTTI, no dynamic allocation.
- No standard library that allocates or pulls in I/O.
- Reference rule set: MISRA C++:2023. Deviations are listed here with a reason.
- Allowed standard headers: to be listed as they are needed.
- Stand-ins for C++20 (span, byte order and bit helpers, `static_assert` for driver contracts) are kept in one place each, so the later migration finds them.
