/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      src/leds/render.hpp
 * @brief     The colours of the LED stick, and a frame turned into the bytes of the stick.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: SW-03
 *
 * Defined in docs/led-patterns.md, "Frames and levels" and "The stick on the wire". The bytes are
 * in the order of the wire, green, red, blue, LED 1 first; the colours are written in R, G, B.
 * Nothing includes this header yet: until main does, the checks run only when the header is
 * compiled on its own.
 */

#ifndef NEXUS_SRC_LEDS_RENDER_HPP
#define NEXUS_SRC_LEDS_RENDER_HPP

#include <cstddef>
#include <cstdint>

#include "leds/patterns.hpp"

namespace nexus::fbl::leds {

/* A colour at level 15. */
struct Colour {
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
};

/* The colours of the patterns, about a sixth of the LED's range (docs/led-patterns.md). */
namespace colours {
inline constexpr Colour blue{0U, 0U, 40U};
inline constexpr Colour amber{40U, 15U, 0U};
inline constexpr Colour magenta{40U, 0U, 30U};
inline constexpr Colour green{0U, 40U, 0U};
}  /* namespace colours */

/* The bytes of one frame of the stick, as they go on the wire. */
inline constexpr std::size_t bytesPerLed = 3U;
inline constexpr std::size_t stickBytes = ledCount * bytesPerLed;

struct StickBytes {
    std::uint8_t bytes[stickBytes];
};

/* The level at which a LED shows its colour as written. */
inline constexpr std::uint32_t fullLevel = levelMask;

/*
 * One channel of a colour at a level: channel * ledLevel / 15, rounded down. Multiplied first: the
 * other order, channel / 15 * ledLevel, would turn every channel below 15 into 0. A level above 15
 * cannot come out of a frame; it is held at 15, so the result always fits a byte.
 */
constexpr std::uint8_t scale(std::uint8_t channel, std::uint32_t ledLevel) noexcept
{
    std::uint32_t held = ledLevel;
    if (held > fullLevel) {
        held = fullLevel;
    }
    /* At most 255 * 15 / 15 = 255: the cast back to a byte loses nothing. */
    return static_cast<std::uint8_t>((static_cast<std::uint32_t>(channel) * held) / fullLevel);
}

/* A frame in a colour: three bytes per LED, green, red, blue, LED 1 first. */
constexpr StickBytes render(std::uint32_t frame, Colour colour) noexcept
{
    /* Zeroed here: a constexpr function of C++17 may not leave a variable uninitialized. */
    StickBytes out{};
    for (std::uint32_t led = 0U; led < ledCount; ++led) {
        const std::uint32_t ledLevel = level(frame, led);
        const std::size_t first = static_cast<std::size_t>(led) * bytesPerLed;
        out.bytes[first] = scale(colour.green, ledLevel);
        out.bytes[first + 1U] = scale(colour.red, ledLevel);
        out.bytes[first + 2U] = scale(colour.blue, ledLevel);
    }
    return out;
}

/* The checks of exercise 2. */
static_assert(scale(40U, 15U) == 40U, "scale: level 15 is the colour");
static_assert(scale(40U, 6U) == 16U, "scale: amber red at level 6");
static_assert(scale(15U, 6U) == 6U, "scale: amber green at level 6");
static_assert(scale(40U, 0U) == 0U, "scale: level 0 is off");

static_assert(render(0x0000000FU, colours::amber).bytes[0] == 15U, "render: green first");
static_assert(render(0x0000000FU, colours::amber).bytes[1] == 40U, "render: then red");
static_assert(render(0x0000000FU, colours::amber).bytes[2] == 0U, "render: then blue");
static_assert(render(0x60000000U, colours::amber).bytes[21] == 6U, "render: LED 8 last, its green");
static_assert(render(0x60000000U, colours::amber).bytes[22] == 16U, "render: LED 8 last, its red");
static_assert(render(0x00000000U, colours::magenta).bytes[0] == 0U, "render: frame 0, first byte");
static_assert(render(0x00000000U, colours::magenta).bytes[stickBytes - 1U] == 0U, "render: frame 0, last byte");

}  /* namespace nexus::fbl::leds */

#endif
