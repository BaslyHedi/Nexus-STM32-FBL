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

#include "stm32f407/leds/leds.hpp"

/* With -ffreestanding main is not special to the compiler, so -Wmissing-declarations wants this. */
int main();

int main()
{
    using nexus::fbl::stm32f407::Leds;

    Leds::init();
    Leds::write(0b0101U); /* green and red */

    for (;;) {
    }
}
