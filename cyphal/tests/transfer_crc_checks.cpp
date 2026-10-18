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
 * the Cyphal Specification v1.0, section A.1. The file holds no code: it is compiled only for
 * the checks. Why each check is there: cyphal/README.md.
 */

#include "nexus/cyphal/transfer_crc.hpp"
#include <type_traits>

/* Check 1 */
static_assert(nexus::cyphal::TransferCrc().value() == 0xFFFFU, "Wrong initial value of the CRC ");

/* Check 2 */
constexpr std::uint8_t data[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
constexpr std::size_t dataSize = std::extent_v<decltype(data)>;
constexpr auto lambdaCheck2 = [](){
    nexus::cyphal::TransferCrc crc;
    crc.add(data, dataSize);
    return crc.value();
};
static_assert(lambdaCheck2() == 0x29B1U, "Wrong CRC value for the given data");

/* Check 3 */
constexpr auto lambdaCheck3 = [](){
    nexus::cyphal::TransferCrc crc;
    const std::uint8_t splitpoint = 4U;
    crc.add(data, splitpoint);
    crc.add(&data[splitpoint], dataSize - splitpoint);
    return crc.value();
};

static_assert(lambdaCheck3() == 0x29B1U, "Wrong CRC value for 2 steps of the given data");

/* Check 4 */
constexpr auto lambdaCheck4 = [](){
    nexus::cyphal::TransferCrc crc;
    crc.add(data, dataSize);
    const std::uint16_t crcValue = crc.value();
    crc.add(static_cast<std::uint8_t>(crcValue >> 8U));
    crc.add(static_cast<std::uint8_t>(crcValue & 0xFFU));
    return crc.value();
};

static_assert(lambdaCheck4() == 0x0000U, "Wrong Calculation expecting 0");