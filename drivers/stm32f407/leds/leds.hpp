/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      drivers/stm32f407/leds/leds.hpp
 * @brief     The four user LEDs of the Discovery board, PD12 to PD15.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: none
 *
 * Bit 0 to bit 3 of a mask are green, orange, red and blue (docs/led-patterns.md). A set bit is
 * an LED on.
 */

#ifndef NEXUS_DRIVERS_STM32F407_LEDS_LEDS_HPP
#define NEXUS_DRIVERS_STM32F407_LEDS_LEDS_HPP

#include <cstdint>

namespace nexus::fbl::stm32f407 {

struct Leds {
    static void init();
    static void write(std::uint32_t mask);
};

}  /* namespace nexus::fbl::stm32f407 */

#endif
