# Nexus-STM32-FBL

The bare-metal flash bootloader of the STM32H533 (NUCLEO-H533RE, HW-22): receives an application image over Cyphal/CAN, verifies it and jumps to it (SW-01).

Consumes a Nexus-CAN-Utils release for the bootloader identifiers. Small by design: no RTOS.

## Conventions

- Decisions are made in Nexus-Platform (`decisions/`). Cite their IDs here; do not restate them.
- **No em-dashes anywhere.** Use a spaced hyphen, a colon or a comma.
- Every source file opens with the header of [`docs/file-headers.md`](docs/file-headers.md). C++ comments are `/* */` only, never `//`.
- Branch before the first edit; one branch per session; merge through a PR on the owner's go.
- The project's goal is learning, not speed. No dates in plans.
