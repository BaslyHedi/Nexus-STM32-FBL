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

    const std::uint32_t before = frame_;

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
    return frame_ != before;
}

void Player::show() noexcept
{
    frame_ = chase(pattern_->step(index_).frame, turn_);
}

}  /* namespace nexus::fbl::leds */
