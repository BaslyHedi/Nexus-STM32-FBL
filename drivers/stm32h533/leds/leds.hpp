/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      drivers/stm32h533/leds/leds.hpp
 * @brief     The user LED of the Nucleo board, LD2 on PA5.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: HW-22
 *
 * LD2 shows LED 1 of a frame: it is on when the level in bits 0 to 3 is above 0, and the other
 * LEDs of the frame are ignored (docs/led-patterns.md).
 */

#ifndef NEXUS_DRIVERS_STM32H533_LEDS_LEDS_HPP
#define NEXUS_DRIVERS_STM32H533_LEDS_LEDS_HPP

#include <cstdint>

namespace nexus::fbl::stm32h533 {

struct Leds {
    static void init();
    static void write(std::uint32_t frame);
};

}  /* namespace nexus::fbl::stm32h533 */

#endif
