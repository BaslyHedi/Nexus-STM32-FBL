# CMake

The build of the FBL image. The root `CMakeLists.txt` only wires the pieces together; each concern has its own file.

## Build

```
git submodule update --init
cmake --preset fbl
cmake --build --preset fbl
```

Needs `arm-none-eabi-gcc` (tested with 14.2.1), CMake 3.25 or newer and Ninja. The result is in `build/output/`:

| File | Content |
|---|---|
| `FBL_Nexus.elf` | for the debugger and the size tools |
| `FBL_Nexus.map` | link map |
| `FBL_Nexus.srec` | the image for the ST-LINK, S3 records |
| `FBL_Nexus_v1_00-01-00.srec` | the same image under its versioned name |

## Files

| File | Owns |
|---|---|
| `project-settings.cmake` | software type, platform name and version, software version, target and output file names, linker script path. Included before `project()`. |
| `toolchain-arm-none-eabi.cmake` | the compiler and its tools, the CPU flags (`-mcpu=cortex-m4 -mthumb -mfloat-abi=soft`, SW-05) |
| `compile-options.cmake` | the `nexus_options` interface library: C++17 and the freestanding subset (SW-03), warnings, link options; and `nexus_use_linker_script()` |
| `version.cmake` | checks the settings against the version record layout, builds the strings, generates `version.hpp` |
| `post-build.cmake` | after the link: the `.init_array` check, the SREC files, the size report |
| `check-init-array.cmake` | fails the build when `.init_array` is not empty (SW-03) |

Each folder with sources has its own `CMakeLists.txt` (`version/`, `drivers/stm32f407/startup/`, `src/boot/`, `third_party/`).

## Things to know

- **The two SREC files differ only in name.** The S0 header record carries the file name, so `objcopy` runs twice, from `build/output/` with a relative name. With a full path the header would hold the path of the build machine.
- **The check comes before the SREC files.** A build that fails the `.init_array` check leaves no SREC.
- **Start-up code and the version record are OBJECT libraries.** Nothing calls them (the linker script's `ENTRY` and the record's `KEEP` are the only references), so a static library would drop them.
- **`-nostdlib` and `-lgcc`.** There is no C library: a call to `malloc`, `operator new` or `memcpy` fails to link, which is the intent (SW-03).
- **Warnings are errors** by default. Switch off with `-DNEXUS_WARNINGS_AS_ERRORS=OFF` while experimenting.
- **With `-ffreestanding`, `main` is not special to the compiler.** `src/boot/main.cpp` declares it once so `-Wmissing-declarations` stays on.
