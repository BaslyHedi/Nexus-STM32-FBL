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
 * Bit 0 of a mask is LD2 (green); the other bits are ignored. A set bit is the LED on. The
 * LED strip component will replace this driver's role in the patterns (docs/led-patterns.md).
 */

#ifndef NEXUS_DRIVERS_STM32H533_LEDS_LEDS_HPP
#define NEXUS_DRIVERS_STM32H533_LEDS_LEDS_HPP

#include <cstdint>

namespace nexus::fbl::stm32h533 {

struct Leds {
    static void init();
    static void write(std::uint32_t mask);
};

}  /* namespace nexus::fbl::stm32h533 */

#endif
