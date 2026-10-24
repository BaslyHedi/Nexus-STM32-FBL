/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      cyphal/tests/transfer_crc_checks.cpp
 * @brief     Compile-time checks of the Cyphal transfer CRC.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: SW-02, SW-03
 *
 * Each check is a static_assert, so a wrong CRC stops the build. The expected values come from
 * the Cyphal Specification v1.0, sections A.1 and 4.2.3. The file holds no code: it is compiled
 * only for the checks. Why each check is there: cyphal/README.md.
 */

#include "nexus/cyphal/transfer_crc.hpp"
#include <type_traits>

/* Check 1 */
static_assert(nexus::cyphal::TransferCrc().value() == 0xFFFFU, "Wrong initial value of the CRC ");

/* Check 2 */
constexpr std::uint8_t checkString[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
constexpr std::size_t checkStringSize = std::extent_v<decltype(checkString)>;
constexpr auto crcOfCheckString = [](){
    nexus::cyphal::TransferCrc crc;
    crc.add(checkString, checkStringSize);
    return crc.value();
};
static_assert(crcOfCheckString() == 0x29B1U, "Wrong CRC value for the given data");

/* Check 3 */
constexpr auto crcOfCheckStringInTwoParts = [](){
    nexus::cyphal::TransferCrc crc;
    constexpr std::size_t splitPoint = 4U;
    crc.add(checkString, splitPoint);
    crc.add(&checkString[splitPoint], checkStringSize - splitPoint);
    return crc.value();
};

static_assert(crcOfCheckStringInTwoParts() == 0x29B1U,
              "Wrong CRC value for 2 steps of the given data");

/* Check 4 */
constexpr auto crcOfCheckStringWithItsCrc = [](){
    nexus::cyphal::TransferCrc crc;
    crc.add(checkString, checkStringSize);
    const std::uint16_t crcValue = crc.value();
    crc.add(static_cast<std::uint8_t>(crcValue >> 8U));
    crc.add(static_cast<std::uint8_t>(crcValue & 0xFFU));
    return crc.value();
};

static_assert(crcOfCheckStringWithItsCrc() == 0x0000U,
              "The check string followed by its CRC, high byte first, does not give 0");

/* Check 5: a real transfer, the GetInfo response of node 42 in spec section 4.2.3. Each frame is
 * kept as it is on the bus: data field and DLC, tail byte included. The CRC is split over the
 * last two frames: 9A ends frame 10, E7 starts frame 11. */
struct CanFrame {
    std::uint8_t data[8];
    std::size_t length;
};

constexpr CanFrame getInfoResponse[] = {
    {{0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0xA1}, 8U},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}, 8U},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21}, 8U},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}, 8U},
    {{0x00, 0x00, 0x24, 0x6F, 0x72, 0x67, 0x2E, 0x21}, 8U},
    {{0x75, 0x61, 0x76, 0x63, 0x61, 0x6E, 0x2E, 0x01}, 8U},
    {{0x70, 0x79, 0x75, 0x61, 0x76, 0x63, 0x61, 0x21}, 8U},
    {{0x6E, 0x2E, 0x64, 0x65, 0x6D, 0x6F, 0x2E, 0x01}, 8U},
    {{0x62, 0x61, 0x73, 0x69, 0x63, 0x5F, 0x75, 0x21}, 8U},
    {{0x73, 0x61, 0x67, 0x65, 0x00, 0x00, 0x9A, 0x01}, 8U},
    {{0xE7, 0x61}, 2U},
};

constexpr std::size_t tailByteSize = 1U;
constexpr std::size_t transferCrcSize = 2U;
/* Nine full frames of 7 payload bytes, then 6 in frame 10 before the CRC. */
constexpr std::size_t getInfoPayloadSize = 69U;

/* Feeds the first byteCount bytes of the transfer, frame by frame, as a receiver would. */
constexpr auto crcOfGetInfoResponse = [](std::size_t byteCount) {
    nexus::cyphal::TransferCrc crc;
    for (const CanFrame& frame : getInfoResponse) {
        const std::size_t frameBytes = frame.length - tailByteSize;
        const std::size_t count = (byteCount < frameBytes) ? byteCount : frameBytes;
        crc.add(frame.data, count);
        byteCount -= count;
    }
    return crc.value();
};

static_assert(crcOfGetInfoResponse(getInfoPayloadSize) == 0x9AE7U,
              "Wrong CRC of the GetInfo response of spec section 4.2.3");
static_assert(crcOfGetInfoResponse(getInfoPayloadSize + transferCrcSize) == 0x0000U,
              "GetInfo response with its CRC does not give 0, as a receiver checks it");
