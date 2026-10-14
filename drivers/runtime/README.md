# Runtime

`memory.cpp`: `memcpy`, `memmove`, `memset` and `memcmp`. GCC requires a freestanding environment to provide them and may emit calls to them on its own, to clear or copy an object. The image links no C library (SW-03), so they are here.

## Rules for this file

- Built with `-fno-tree-loop-distribute-patterns` (`drivers/CMakeLists.txt`). Without it GCC turns the loop of `memset` into a call to `memset`, and the function calls itself until the stack overflows.
- The functions are in `.text`, so in the SRAM at run time (SW-07). The start-up code must not call them before the copy of `.ram_image`: `startup.cpp` is built with the same option for that reason.
- Byte by byte. A word-wise version only if a measurement shows the time matters.
