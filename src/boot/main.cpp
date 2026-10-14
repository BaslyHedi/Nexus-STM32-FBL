/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      src/boot/main.cpp
 * @brief     Entry point of the bootloader.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: none
 *
 * Stub: the bootloader logic is not written yet. It only lets the image link and start.
 */

#include <cstdint>
#include "stm32f407/leds/leds.hpp"
#include "stm32f407/tick/tick.hpp"

/* With -ffreestanding main is not special to the compiler, so -Wmissing-declarations wants this. */
int main();

int main()
{
    using nexus::fbl::stm32f407::Leds;
    using nexus::fbl::stm32f407::Tick;

    /* Reset clock: HSI, 16 MHz, until the clock driver exists. */
    constexpr std::uint32_t coreClockHz = 16000000U;

    Leds::init();
    Tick::init(coreClockHz);

    std::uint32_t mask = 0xFu;
    std::uint32_t start = Tick::now();

    for (;;) {
        if ((Tick::now() - start) >= 1000U) {
            start += 1000U;
            mask ^= 0xFu;
            Leds::write(mask);
        }
    }

}
