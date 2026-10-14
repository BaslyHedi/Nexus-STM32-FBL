/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      src/leds/player.cpp
 * @brief     Plays an LED pattern against a millisecond clock.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: none
 */

#include "leds/player.hpp"

namespace nexus::fbl::leds {

namespace {

constexpr std::uint32_t ledCount = 4U;
constexpr std::uint32_t allLeds = 0xFU;

/* Moves every LED of the mask turn places clockwise, blue wrapping round to green. */
std::uint32_t rotate(std::uint32_t mask, std::uint32_t turn) noexcept
{
    return ((mask << turn) | (mask >> (ledCount - turn))) & allLeds;
}

}  /* namespace */

void Player::start(const Pattern& pattern, std::uint32_t nowMs) noexcept
{
    pattern_ = &pattern;
    index_ = 0U;
    turn_ = 0U;
    stepStartMs_ = nowMs;
    show();
}

bool Player::update(std::uint32_t nowMs) noexcept
{
    if (pattern_ == nullptr) {
        return false;
    }

    const std::uint32_t before = mask_;

    /* while and not if: after a stall the player walks through the missed steps and is on time
     * again, instead of running late from then on. */
    while ((nowMs - stepStartMs_) >= pattern_->step(index_).durationMs) {
        stepStartMs_ += pattern_->step(index_).durationMs;
        ++index_;
        if (index_ == pattern_->size()) {
            index_ = 0U;
            if (pattern_->motion() == Motion::chase) {
                turn_ = (turn_ + 1U) % ledCount;
            }
        }
    }

    show();
    return mask_ != before;
}

void Player::show() noexcept
{
    mask_ = rotate(pattern_->step(index_).mask, turn_);
}

}  /* namespace nexus::fbl::leds */
