# CMake

The build of the FBL image. The root `CMakeLists.txt` only wires the pieces together; each concern has its own file.

## Build

```
git submodule update --init
cmake --preset fbl
cmake --build --preset fbl                    # both variants
cmake --build --preset fbl --target fbl_debug # one of them: fbl or fbl_debug
```

Needs `arm-none-eabi-gcc` (tested with 14.2.1), CMake 3.25 or newer and Ninja. One preset builds two variants of the same sources, side by side:

| Variant | Target | Options | Use |
|---|---|---|---|
| release | `fbl` | `-Os -g3 -DNDEBUG` | the image that ships |
| debug | `fbl_debug` | `-Og -g3` | stepping in the debugger |

The result is in `build/output/`:

| File | Content |
|---|---|
| `FBL_Nexus.elf` | release image, for the debugger and the size tools |
| `FBL_Nexus.map` | release link map |
| `FBL_Nexus.srec` | the release image for the ST-LINK, S3 records |
| `FBL_Nexus_v1_00-01-00.srec` | the same image under its versioned name |
| `FBL_Nexus_debug.elf` | debug image, flashed by the debugger from this file |
| `FBL_Nexus_debug.map` | debug link map |

## Files

| File | Owns |
|---|---|
| `project-settings.cmake` | software type, platform name and version, software version, the variants and their target and output file names, linker script path. Included before `project()`. |
| `toolchain-arm-none-eabi.cmake` | the compiler and its tools, the CPU flags (`-mcpu=cortex-m4 -mthumb -mfloat-abi=soft`, SW-05) |
| `compile-options.cmake` | the `nexus_options` interface library: C++17 and the freestanding subset (SW-03), warnings, link options; one `nexus_variant_<variant>` interface library per variant with its optimisation; and `nexus_use_linker_script()` |
| `version.cmake` | checks the settings against the version record layout, builds the strings, generates `version.hpp` |
| `post-build.cmake` | after the link: the `.init_array` check, the SREC files (release only), the size report |
| `check-init-array.cmake` | fails the build when `.preinit_array`, `.init_array` or `.ctors` is not empty, or cannot be read (SW-03) |

Each top-level folder with sources has one `CMakeLists.txt` for the whole folder, with paths relative to it (`version/`, `drivers/`, `src/`, `third_party/`). A subfolder has none.

## Things to know

- **The optimisation belongs to the variant, not to the build type.** `CMAKE_BUILD_TYPE` adds its flags to every target, so the configure fails when it is set. A build tree from before the variants still has `MinSizeRel` in its cache: `cmake --preset fbl --fresh`.
- **`-g3` on both variants changes nothing in the image.** Debug information goes into the `.elf` only, and GCC generates the same code with or without `-g`. The release SREC is byte-identical to one built without it.
- **Only the release image gets SREC files.** The SREC is what goes on a board; an SREC of the debug image next to it could be flashed by mistake.
- **Both variants carry the same version record.** The record (SW-08) has no field for the variant; a debug image on a board reads as the release of the same version.
- **Every source is compiled once per variant.** The OBJECT libraries are `nexus_startup_<variant>` and `nexus_version_<variant>`, and a folder that adds sources to the image adds them to each `NEXUS_TARGET_<variant>` (`src/CMakeLists.txt`).

- **The two SREC files differ only in name.** The S0 header record carries the file name, so `objcopy` runs twice, from `build/output/` with a relative name. With a full path the header would hold the path of the build machine.
- **The check comes before the SREC files.** The old SREC files are deleted first, so a build that fails the constructor check leaves no SREC, neither a new one nor one of the last good build.
- **Strict `-std=c++17`.** `CMAKE_CXX_EXTENSIONS` is off: GCC 14 defaults to `gnu++17`, and CMake then adds no `-std` flag at all.
- **Start-up code and the version record are OBJECT libraries.** Nothing calls them (the linker script's `ENTRY` and the record's `KEEP` are the only references), so a static library would drop them.
- **`-nostdlib` and `-lgcc`.** There is no C library: a call to `malloc`, `operator new` or `memcpy` fails to link, which is the intent (SW-03).
- **Warnings are errors** by default. Switch off with `-DNEXUS_WARNINGS_AS_ERRORS=OFF` while experimenting.
- **With `-ffreestanding`, `main` is not special to the compiler.** `src/boot/main.cpp` declares it once so `-Wmissing-declarations` stays on.
