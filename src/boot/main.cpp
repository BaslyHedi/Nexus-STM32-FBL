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
 * Temporary: plays the FBL idle pattern on LD2 (T1.3) until the update state machine exists. Only
 * bit 0 of the pattern reaches the Nucleo's single LED; the LED strip patterns come later.
 */

#include <cstdint>

#include "stm32h533/leds/leds.hpp"
#include "stm32h533/tick/tick.hpp"
#include "leds/patterns.hpp"
#include "leds/player.hpp"

/* With -ffreestanding main is not special to the compiler, so -Wmissing-declarations wants this. */
int main();

int main()
{
    using nexus::fbl::leds::Player;
    using nexus::fbl::stm32h533::Leds;
    using nexus::fbl::stm32h533::Tick;

    /* Reset clock: HSI, 32 MHz (HSIDIV = 2, RM0481 section 11.4.2), until the clock driver exists. */
    constexpr std::uint32_t coreClockHz = 32000000U;

    Leds::init();
    Tick::init(coreClockHz);

    Player player;
    player.start(nexus::fbl::leds::idle, Tick::now());
    Leds::write(player.mask());

    for (;;) {
        if (player.update(Tick::now())) {
            Leds::write(player.mask());
        }
    }
}
