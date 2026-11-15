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
 * there, so the two can be compared line by line. A step holds a frame of eight 4-bit levels, one
 * per LED of the stick; LD2 shows LED 1. The colour is not part of a pattern: LD2 has none.
 */

#ifndef NEXUS_SRC_LEDS_PATTERNS_HPP
#define NEXUS_SRC_LEDS_PATTERNS_HPP

#include <cstddef>
#include <cstdint>

namespace nexus::fbl::leds {

/* The stick has eight LEDs; a frame holds one 4-bit level per LED, LED 1 in bits 0 to 3. */
inline constexpr std::uint32_t ledCount = 8U;
inline constexpr std::uint32_t levelBits = 4U;
inline constexpr std::uint32_t levelMask = 0xFU;

/* One step: the level of each LED, and for how long. Level 0 is off, 15 the full colour. */
struct Step {
    std::uint32_t frame;
    std::uint32_t durationMs;
};

/* still: the steps are played as written. chase: at the end of each pass every level moves on one
 * LED, towards LED 8, so the table holds the part of the first pass only. */
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

/* The level of one LED, led counted from 0 (LED 1). */
constexpr std::uint32_t level(std::uint32_t frame, std::uint32_t led) noexcept
{
    return (frame >> (led * levelBits)) & levelMask;
}

/*
 * Moves every level turn LEDs on, towards LED 8, LED 8 wrapping to LED 1. Turn 0 is returned as it
 * is: the rotation would otherwise shift a 32-bit value by 32, which is undefined behaviour.
 */
constexpr std::uint32_t chase(std::uint32_t frame, std::uint32_t turn) noexcept
{
    const std::uint32_t shift = (turn % ledCount) * levelBits;
    if (shift == 0U) {
        return frame;
    }
    return (frame << shift) | (frame >> ((ledCount * levelBits) - shift));
}

static_assert(chase(0x6200000FU, 1U) == 0x200000F6U, "chase: the comet moves on one LED");
static_assert(chase(0x0000000FU, 7U) == 0xF0000000U, "chase: LED 1 reaches LED 8 in seven turns");
static_assert(chase(0x6200000FU, 8U) == 0x6200000FU, "chase: eight turns bring the frame back");
static_assert(chase(0x6200000FU, 0U) == 0x6200000FU, "chase: turn 0 leaves the frame as it is");
static_assert(level(0x6200000FU, 7U) == 6U, "level: LED 8 is bits 28 to 31");

/* The time of one pass through the table. */
constexpr std::uint32_t passDurationMs(const Pattern& pattern) noexcept
{
    std::uint32_t total = 0U;
    for (std::size_t i = 0U; i < pattern.size(); ++i) {
        total += pattern.step(i).durationMs;
    }
    return total;
}

/* True when LD2, which shows only LED 1, sees the pattern switch both on and off. */
constexpr bool blinksOnLd2(const Pattern& pattern) noexcept
{
    bool on = false;
    bool off = false;
    for (std::size_t i = 0U; i < pattern.size(); ++i) {
        if (level(pattern.step(i).frame, 0U) != 0U) {
            on = true;
        } else {
            off = true;
        }
    }
    return on && off;
}

/* FBL idle: the burst. Chased on the stick, still on LD2, from the same table. */
inline constexpr Step idleSteps[] = {
    {0x0000000FU, 70U},
    {0x00000000U, 30U},
    {0x0000000FU, 70U},
    {0x00000000U, 30U},
    {0x0000000FU, 70U},
    {0x00000000U, 30U},
    {0x0000000FU, 70U},
    {0x00000000U, 30U},
    {0x0000000FU, 70U},
    {0x00000000U, 30U},
    {0x0000000FU, 70U},
    {0x00000000U, 230U},
};

inline constexpr Pattern idleStick{idleSteps, Motion::chase};
inline constexpr Pattern idleLd2{idleSteps, Motion::still};

static_assert(idleStick.size() == 12U, "FBL idle: twelve steps, docs/led-patterns.md");
static_assert(passDurationMs(idleStick) == 800U, "FBL idle: a burst lasts 800 ms, docs/led-patterns.md");
static_assert(blinksOnLd2(idleLd2), "FBL idle: LD2 blinks");

/* FBL receiving: the comet on the stick, head on LED 1, tail on LED 8 and LED 7. */
inline constexpr Step receivingStickSteps[] = {
    {0x6200000FU, 100U},
};

inline constexpr Pattern receivingStick{receivingStickSteps, Motion::chase};

static_assert(receivingStick.size() == 1U, "FBL receiving: one step on the stick, docs/led-patterns.md");
static_assert(passDurationMs(receivingStick) == 100U, "FBL receiving: 100 ms per LED, docs/led-patterns.md");

/* FBL receiving on LD2: 100 ms on, 300 ms off, what LD2 showed of the four-LED circle. */
inline constexpr Step receivingLd2Steps[] = {
    {0x0000000FU, 100U},
    {0x00000000U, 300U},
};

inline constexpr Pattern receivingLd2{receivingLd2Steps, Motion::still};

static_assert(receivingLd2.size() == 2U, "FBL receiving: two steps on LD2, docs/led-patterns.md");
static_assert(passDurationMs(receivingLd2) == 400U, "FBL receiving: 100 ms on, 300 ms off on LD2, docs/led-patterns.md");
static_assert(blinksOnLd2(receivingLd2), "FBL receiving: LD2 blinks");

/* FBL-Updater: all eight flash together, LD2 with them. One pattern for both. */
inline constexpr Step updaterSteps[] = {
    {0xFFFFFFFFU, 100U},
    {0x00000000U, 100U},
};

inline constexpr Pattern updater{updaterSteps, Motion::still};

static_assert(updater.size() == 2U, "FBL-Updater: two steps, docs/led-patterns.md");
static_assert(passDurationMs(updater) == 200U, "FBL-Updater: 100 ms on, 100 ms off, docs/led-patterns.md");
static_assert(blinksOnLd2(updater), "FBL-Updater: LD2 blinks");

}  /* namespace nexus::fbl::leds */

#endif
