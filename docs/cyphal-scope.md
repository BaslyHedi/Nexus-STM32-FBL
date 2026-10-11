# Cyphal scope of the FBL

What the in-house stack has to do for the download, and nothing more (SW-02). IDs and versions are checked against the Cyphal Specification v1.0, revision 2025-05-16 (OpenCyphal, CC BY 4.0). On the author's machine the copy is in the untracked `vendor/datasheets/` of Nexus-Platform.

| Port | Type version | Role in the FBL | Direction | Multi-frame |
|---|---|---|---|---|
| `uavcan.node.Heartbeat`, subject 7509 | 1.0 | Announce the node, mode software update | publish | no |
| `uavcan.node.GetInfo`, service 430 | 1.0 | Report versions and name (T1.1, T1.2) | serve | response |
| `uavcan.node.ExecuteCommand`, service 435 | 1.3 | Receive `COMMAND_BEGIN_SOFTWARE_UPDATE` (65533), the parameter is the file path | serve | request |
| `uavcan.file.Read`, service 408 | 1.1 | Pull the image block by block | client | response |

**Update procedure** (spec section 5.3.9 and the ExecuteCommand type). The roles swap: the host is the client of ExecuteCommand and the server of `file.Read`. The node answers SUCCESS or an error status such as BAD_STATE, sets the Heartbeat mode to software update while it downloads, and repeats `file.Read` until the file is complete. A second request while one is running aborts and restarts it, unless it names the same file.

**Limits that keep it small.** One node. One reassembly session at a time. No dynamic allocation. Classic CAN only.

**Layers, in build order.** Transfer CRC. CAN identifier. Transmit with segmentation. Receive with reassembly and the transfer-ID rules. Serialization of the four types. The node.

**Checked against.** The specification's examples on the host, and `yakut` over SocketCAN on the bench.
