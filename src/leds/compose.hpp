/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      src/leds/compose.hpp
 * @brief     The progress fill of the LED stick, and two frames combined into one.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: SW-03
 *
 * Pure functions of their arguments, constexpr, so the checks below run at compile time. Defined in
 * docs/led-patterns.md, "The progress fill". Nothing includes this header yet: until main does, the
 * checks run only when the header is compiled on its own.
 */

#ifndef NEXUS_SRC_LEDS_COMPOSE_HPP
#define NEXUS_SRC_LEDS_COMPOSE_HPP

#include <cstdint>

#include "leds/patterns.hpp"

namespace nexus::fbl::leds {

/* The level of a full LED of the fill, and the number of fill steps on the stick. */
inline constexpr std::uint32_t fillPerLed = 4U;
inline constexpr std::uint32_t fillSteps = ledCount * fillPerLed;

/*
 * The fill, from 0 to fillSteps: received * fillSteps / size, rounded down. A size of 0 means the
 * size is not known: no fill. More bytes than the size: a full fill.
 *
 * received * fillSteps fits 32 bits while received stays under 128 MB. The multiplication is only
 * reached when received is below size, and size is bounded by the application region (256 KB)
 * before the download starts (docs/cyphal-scope.md). The static_assert on that bound comes with the
 * memory map header (SW-12), when the region length becomes a compile-time constant.
 */
constexpr std::uint32_t fill(std::uint32_t received, std::uint32_t size) noexcept
{
    /* Unknown size (0): no fill. */
    std::uint32_t steps = 0U;
    /* Tested first: a division by 0 is not allowed, and in a constant expression it stops the build. */
    if (size != 0U) {
        /* Before the multiplication: it bounds received by size, which the overflow note relies on. */
        if (received >= size) {
            steps = fillSteps;
        } else {
            steps = (received * fillSteps) / size;
        }
    }
    return steps;
}

/* The frame of a fill: LED i, counted from 0, at steps - fillPerLed * i, held between 0 and fillPerLed. */
constexpr std::uint32_t fillFrame(std::uint32_t steps) noexcept
{
    std::uint32_t frame = 0U;
    for (std::uint32_t led = 0U; led < ledCount; ++led) {
        const std::uint32_t before = led * fillPerLed;
        /* Compared before the subtraction: steps - before would wrap round in unsigned arithmetic. */
        std::uint32_t ledLevel = 0U;
        if (steps > before) {
            ledLevel = steps - before;
            if (ledLevel > fillPerLed) {
                ledLevel = fillPerLed;
            }
        }
        frame |= ledLevel << (led * levelBits);
    }
    return frame;
}

/* Two frames into one, LED by LED: the higher level wins. */
constexpr std::uint32_t combine(std::uint32_t first, std::uint32_t second) noexcept
{
    std::uint32_t frame = 0U;
    for (std::uint32_t led = 0U; led < ledCount; ++led) {
        const std::uint32_t a = level(first, led);
        const std::uint32_t b = level(second, led);
        frame |= ((a > b) ? a : b) << (led * levelBits);
    }
    return frame;
}

/* The checks of exercise 2. */
static_assert(fill(0U, 1000U) == 0U, "fill: nothing received");
static_assert(fill(999U, 1000U) == 31U, "fill: rounded down");
static_assert(fill(1000U, 1000U) == 32U, "fill: everything received");
static_assert(fill(2000U, 1000U) == 32U, "fill: more than the size is held at full");
static_assert(fill(500U, 0U) == 0U, "fill: unknown size, no fill");

static_assert(fillFrame(0U) == 0x00000000U, "fillFrame: empty");
static_assert(fillFrame(5U) == 0x00000014U, "fillFrame: LED 1 full, LED 2 at 1");
static_assert(fillFrame(31U) == 0x34444444U, "fillFrame: LED 8 at 3");
static_assert(fillFrame(32U) == 0x44444444U, "fillFrame: full");

static_assert(combine(0x6200000FU, 0x44444444U) == 0x6444444FU, "combine: the comet over a full fill");

}  /* namespace nexus::fbl::leds */

#endif
