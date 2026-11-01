/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      drivers/stm32h533/tick/tick.hpp
 * @brief     Millisecond time base from the SysTick interrupt.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: none
 *
 * now() wraps after 49.7 days. Compare times by subtraction, now() - start >= duration, which
 * stays correct across the wrap.
 */

#ifndef NEXUS_DRIVERS_STM32H533_TICK_TICK_HPP
#define NEXUS_DRIVERS_STM32H533_TICK_TICK_HPP

#include <cstdint>

namespace nexus::fbl::stm32h533 {

struct Tick {
    static void init(std::uint32_t coreClockHz);
    static std::uint32_t now();
};

}  /* namespace nexus::fbl::stm32h533 */

#endif
