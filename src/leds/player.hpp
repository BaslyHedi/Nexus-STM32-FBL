/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      src/leds/player.hpp
 * @brief     Plays an LED pattern against a millisecond clock.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: none
 *
 * The player does not drive the LEDs: it says which mask is due, and the caller writes it. That
 * keeps it free of the hardware, so a host test can drive it with any sequence of times.
 */

#ifndef NEXUS_SRC_LEDS_PLAYER_HPP
#define NEXUS_SRC_LEDS_PLAYER_HPP

#include <cstddef>
#include <cstdint>

#include "leds/patterns.hpp"

namespace nexus::fbl::leds {

class Player {
public:
    /* Starts the pattern from its first step at nowMs. The player keeps a pointer to it. */
    void start(const Pattern& pattern, std::uint32_t nowMs) noexcept;
    /* A temporary would be gone before the next update: refused at compile time. */
    void start(const Pattern&& pattern, std::uint32_t nowMs) = delete;

    /* Advances to the step due at nowMs. True when the mask changed. */
    bool update(std::uint32_t nowMs) noexcept;

    std::uint32_t mask() const noexcept { return mask_; }

private:
    void show() noexcept;

    const Pattern* pattern_ = nullptr;
    std::size_t index_ = 0U;
    std::uint32_t turn_ = 0U;
    std::uint32_t stepStartMs_ = 0U;
    std::uint32_t mask_ = 0U;
};

}  /* namespace nexus::fbl::leds */

#endif
