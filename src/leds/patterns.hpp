/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      src/leds/patterns.hpp
 * @brief     The LED patterns of the bootloader, as constant tables.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: SW-03
 *
 * The patterns are defined in docs/led-patterns.md. Each table has one row per step of the table
 * there, so the two can be compared line by line.
 */

#ifndef NEXUS_SRC_LEDS_PATTERNS_HPP
#define NEXUS_SRC_LEDS_PATTERNS_HPP

#include <cstddef>
#include <cstdint>

namespace nexus::fbl::leds {

/* One step: which LEDs are on, and for how long. Bit 0 to bit 3 are green, orange, red, blue. */
struct Step {
    std::uint32_t mask;
    std::uint32_t durationMs;
};

/* still: the steps are played as written. chase: at the end of each pass every mask moves one LED
 * clockwise, so the table holds the part of the first LED only. */
enum class Motion { still, chase };

/* A pattern: a fixed table of steps, played in order and repeated. */
class Pattern {
public:
    /* The size is taken from the array, so it cannot disagree with the table. */
    template <std::size_t N>
    constexpr Pattern(const Step (&steps)[N], Motion motion) noexcept
        : steps_(steps), size_(N), motion_(motion)
    {
    }

    constexpr const Step& step(std::size_t index) const noexcept { return steps_[index]; }
    constexpr std::size_t size() const noexcept { return size_; }
    constexpr Motion motion() const noexcept { return motion_; }

private:
    const Step* steps_;
    std::size_t size_;
    Motion motion_;
};

/* The time of one pass through the table. */
constexpr std::uint32_t passDurationMs(const Pattern& pattern) noexcept
{
    std::uint32_t total = 0U;
    for (std::size_t i = 0U; i < pattern.size(); ++i) {
        total += pattern.step(i).durationMs;
    }
    return total;
}

/* True when no mask names a fifth LED. */
constexpr bool usesFourLeds(const Pattern& pattern) noexcept
{
    for (std::size_t i = 0U; i < pattern.size(); ++i) {
        if ((pattern.step(i).mask & ~0xFU) != 0U) {
            return false;
        }
    }
    return true;
}

/*
 * FBL idle: the burst. Still on the Nucleo, whose one LED would show only one pass in four of a
 * chase; the strip brings the chase back (docs/led-patterns.md).
 */
inline constexpr Step idleSteps[] = {
    {0b0001U, 70U},
    {0b0000U, 30U},
    {0b0001U, 70U},
    {0b0000U, 30U},
    {0b0001U, 70U},
    {0b0000U, 30U},
    {0b0001U, 70U},
    {0b0000U, 30U},
    {0b0001U, 70U},
    {0b0000U, 30U},
    {0b0001U, 70U},
    {0b0000U, 230U},
};

inline constexpr Pattern idle{idleSteps, Motion::still};

static_assert(idle.size() == 12U, "FBL idle: twelve steps, docs/led-patterns.md");
static_assert(passDurationMs(idle) == 800U, "FBL idle: a burst lasts 800 ms, docs/led-patterns.md");
static_assert(usesFourLeds(idle), "FBL idle: four LEDs only");

/* FBL receiving: the circle. One step on the first LED, moved on by the chase every pass. */
inline constexpr Step receivingSteps[] = {
    {0b0001U, 100U},
};

inline constexpr Pattern receiving{receivingSteps, Motion::chase};

static_assert(receiving.size() == 1U, "FBL receiving: one step, docs/led-patterns.md");
static_assert(passDurationMs(receiving) == 100U, "FBL receiving: 100 ms per LED, docs/led-patterns.md");
static_assert(usesFourLeds(receiving), "FBL receiving: four LEDs only");

/* FBL-Updater: all four flash together. */
inline constexpr Step updaterSteps[] = {
    {0b1111U, 100U},
    {0b0000U, 100U},
};

inline constexpr Pattern updater{updaterSteps, Motion::still};

static_assert(updater.size() == 2U, "FBL-Updater: two steps, docs/led-patterns.md");
static_assert(passDurationMs(updater) == 200U, "FBL-Updater: 100 ms on, 100 ms off, docs/led-patterns.md");
static_assert(usesFourLeds(updater), "FBL-Updater: four LEDs only");

}  /* namespace nexus::fbl::leds */

#endif
