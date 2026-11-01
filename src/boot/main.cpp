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
 * Temporary: plays the three FBL patterns in turn (T1.3 demo) until the update state machine
 * exists.
 */

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "stm32h533/leds/leds.hpp"
#include "stm32h533/tick/tick.hpp"
#include "leds/patterns.hpp"
#include "leds/player.hpp"

namespace {

/* One scene of the demo: a pattern and how long it plays. */
struct Scene {
    const nexus::fbl::leds::Pattern* pattern;
    std::uint32_t durationMs;
};

constexpr Scene demo[] = {
    {&nexus::fbl::leds::idle, 4000U},
    {&nexus::fbl::leds::receiving, 6000U},
    {&nexus::fbl::leds::updater, 2000U},
};

constexpr std::size_t demoSize = std::extent_v<decltype(demo)>;

}  /* namespace */

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

    std::size_t scene = 0U;
    std::uint32_t sceneStartMs = Tick::now();

    Player player;
    player.start(*demo[scene].pattern, sceneStartMs);
    Leds::write(player.mask());

    for (;;) {
        const std::uint32_t nowMs = Tick::now();

        /* The new pattern starts at the scene boundary, not when the loop notices it: no drift.
         * The write follows start, because update only reports a change from the start mask.
         * while and not if: after a stall longer than a scene, the demo is on time in one pass. */
        while ((nowMs - sceneStartMs) >= demo[scene].durationMs) {
            sceneStartMs += demo[scene].durationMs;
            scene = (scene + 1U) % demoSize;
            player.start(*demo[scene].pattern, sceneStartMs);
            Leds::write(player.mask());
        }

        if (player.update(nowMs)) {
            Leds::write(player.mask());
        }
    }
}
