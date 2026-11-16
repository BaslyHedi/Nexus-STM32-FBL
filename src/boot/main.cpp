/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      src/boot/main.cpp
 * @brief     Entry point of the bootloader.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: HW-22
 *
 * Temporary: plays the FBL idle pattern (T1.3) until the update state machine exists. Two players
 * start on the same tick, one for LD2 and one for the LED stick. Only LD2 is driven: the stick's
 * frames wait for its driver.
 */

#include <cstdint>

#include "stm32h533/leds/leds.hpp"
#include "stm32h533/tick/tick.hpp"
#include "leds/patterns.hpp"
#include "leds/player.hpp"
#include "leds/looks.hpp"

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

    Player ld2;
    Player stick;
    const std::uint32_t startMs = Tick::now();
    ld2.start(nexus::fbl::leds::idleLd2, startMs);
    stick.start(nexus::fbl::leds::idleStick, startMs);
    Leds::write(ld2.frame());

    for (;;) {
        const std::uint32_t nowMs = Tick::now();
        if (ld2.update(nowMs)) {
            Leds::write(ld2.frame());
        }
        /* No driver for the stick yet: its frame is computed and not sent. */
        static_cast<void>(stick.update(nowMs));
    }
}
