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
#include "leds/patterns.hpp"
#include "leds/player.hpp"

/* With -ffreestanding main is not special to the compiler, so -Wmissing-declarations wants this. */
int main();

int main()
{
    using nexus::fbl::leds::Player;
    using nexus::fbl::stm32f407::Leds;
    using nexus::fbl::stm32f407::Tick;

    /* Reset clock: HSI, 16 MHz, until the clock driver exists. */
    constexpr std::uint32_t coreClockHz = 16000000U;

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
