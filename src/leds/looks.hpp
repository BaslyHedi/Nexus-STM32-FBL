/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      src/leds/looks.hpp
 * @brief     What each state of the bootloader shows: its LD2 pattern, its stick pattern, the colour.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: SW-03
 *
 * The table "How the four states differ" of docs/led-patterns.md. The look holds references to the
 * patterns, which are inline constexpr globals: it is constant-initialized, so nothing is added to
 * the .init_array (SW-03). The three states of the FBL; the application has its own.
 */

#ifndef NEXUS_SRC_LEDS_LOOKS_HPP
#define NEXUS_SRC_LEDS_LOOKS_HPP

#include "leds/patterns.hpp"
#include "leds/render.hpp"

namespace nexus::fbl::leds {

struct Look {
    const Pattern& ld2;
    const Pattern& stick;
    Colour colour;
};

inline constexpr Look idleLook{idleLd2, idleStick, colours::blue};
inline constexpr Look receivingLook{receivingLd2, receivingStick, colours::amber};
inline constexpr Look updaterLook{updater, updater, colours::magenta};

}  /* namespace nexus::fbl::leds */

#endif
