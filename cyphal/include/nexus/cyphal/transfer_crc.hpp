/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      cyphal/include/nexus/cyphal/transfer_crc.hpp
 * @brief     The CRC of a multi-frame Cyphal/CAN transfer, computed byte by byte.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: SW-02, SW-03
 *
 * CRC-16/CCITT-FALSE, Cyphal Specification v1.0, section A.1: polynomial 0x1021, initial value
 * 0xFFFF, input and output not reflected, no final XOR. Bitwise, without a table. The bytes can
 * be added in pieces, as the frames of a transfer arrive. The CRC is sent high byte first; a
 * receiver that adds the payload and then the received CRC gets 0 when the transfer is intact.
 */

#ifndef NEXUS_CYPHAL_INCLUDE_NEXUS_CYPHAL_TRANSFER_CRC_HPP
#define NEXUS_CYPHAL_INCLUDE_NEXUS_CYPHAL_TRANSFER_CRC_HPP

#include <cstdint>
#include <cstddef>

namespace nexus::cyphal {

class TransferCrc {
public:
    /* One byte operation */
    constexpr void add(std::uint8_t const byte) noexcept
    {
        std::uint16_t rem = value_ ^ (byte << 8U);
        for (std::uint_fast8_t i = 0U; i < 8U; ++i)
        {
            if ((rem & msbBit_) != 0U)
            {
                rem = static_cast<std::uint16_t>((rem << 1U) ^ polynomial_);
            }
            else
            {
                rem = static_cast<std::uint16_t>(rem << 1U);
            }
        }
        
        value_ = rem;
    }

    /* Multiple bytes operation */
    constexpr void add(const std::uint8_t* data, std::size_t size) noexcept
    {
        if (data != nullptr) {
            for (std::size_t i = 0U; i < size; ++i) {
                add(data[i]);
            }
        }
    }

    /* Getter function for the private member value_ */
    constexpr std::uint16_t value() const noexcept { return value_; }

private:
    /* Constants */
    static constexpr std::uint16_t polynomial_ = 0x1021U;
    static constexpr std::uint16_t initialValue_ = 0xFFFFU;
    static constexpr std::uint16_t msbBit_ = 0x8000U;

    /* Variables */
    std::uint16_t value_ = initialValue_;
};
}   /* namespace nexus::cyphal */

#endif
