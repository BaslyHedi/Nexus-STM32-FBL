# Cyphal stack

The in-house Cyphal/CAN stack (SW-02). Its own build target, with **no dependency on the rest of this repository**, so it can move out and be shared later.

Scope: only what the download needs, see [`../docs/cyphal-scope.md`](../docs/cyphal-scope.md). Layers: transfer CRC, CAN identifier, transmit (segmentation), receive (reassembly), the four standard types.

## Layout

| Folder | Holds |
|---|---|
| `include/nexus/cyphal/` | the public headers, included as `nexus/cyphal/<name>.hpp` |
| `src/` | the sources of the layers that are not header-only (none yet) |
| `tests/` | the checks of each layer |

## Layers

| Layer | Header | Status |
|---|---|---|
| Transfer CRC, CRC-16/CCITT-FALSE (spec section A.1) | `transfer_crc.hpp` | done, checked at compile time |
| CAN identifier | | not started |
| Transmit, segmentation | | not started |
| Receive, reassembly and transfer-ID rules | | not started |
| Serialization of the four types | | not started |

## Checks

Each layer is checked against the specification's own values. Until the host tests of [`../tests/`](../tests/README.md) exist, the checks are `static_assert`s in `tests/`. That works because the layer is `constexpr`. The file is compiled with the image, so a wrong value stops the build.

The transfer CRC is checked against these values:

| Input | Expected | Why |
|---|---|---|
| nothing | `FFFF` | the initial value |
| `"123456789"` | `29B1` | the check value of the spec, section A.1 |
| `"1234"` then `"56789"` | `29B1` | a transfer arrives in frames, so the CRC is fed in pieces |
| `"123456789"`, then `29 B1` | `0000` | the receiver runs the CRC over payload and CRC, then compares to zero. This only works with the CRC appended high byte first, the native big-endian order of the spec |
| The GetInfo response of spec section 4.2.3, 11 frames, fed frame by frame without the tail bytes | `9AE7` | a real transfer from the spec, with the CRC split over the last two frames |
| The same, with its CRC bytes | `0000` | the receiver's check on a real transfer |
