# Cyphal stack

The in-house Cyphal/CAN stack (SW-02). Its own build target, with **no dependency on the rest of this repository**, so it can move out and be shared later.

Scope: only what the download needs, see [`../docs/cyphal-scope.md`](../docs/cyphal-scope.md). Layers: transfer CRC, CAN identifier, transmit (segmentation), receive (reassembly), the four standard types.

Planned subfolders: `include/nexus/cyphal/`, `src/`, `tests/`.
