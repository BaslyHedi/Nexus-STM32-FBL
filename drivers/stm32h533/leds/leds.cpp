/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      drivers/stm32h533/leds/leds.cpp
 * @brief     The user LED of the Nucleo board, LD2 on PA5.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: HW-22
 *
 * UM3121: LD2 is driven through a transistor, so a high level lights it.
 */

#include "stm32h533/leds/leds.hpp"

#include "stm32h533xx.h"

namespace nexus::fbl::stm32h533 {

namespace {

constexpr std::uint32_t ld2Pin = 5U;

}  /* namespace */

void Leds::init()
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    /* The port needs its clock a moment before it is usable. Reading the register back waits for it. */
    static_cast<void>(RCC->AHB2ENR);

    /* Off before the pin becomes an output, so the LED does not flash during the set-up. */
    write(0U);

    /* MODER: two bits per pin, 01 is a general-purpose output. Push-pull, low speed and no pull
     * are the reset values of OTYPER, OSPEEDR and PUPDR. */
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE5_Msk) | GPIO_MODER_MODE5_0;
}

void Leds::write(std::uint32_t mask)
{
    /* BSRR: the low half sets pins, the high half resets them, in one write. */
    GPIOA->BSRR = ((mask & 1U) != 0U) ? (1U << ld2Pin) : (1U << (ld2Pin + 16U));
}

}  /* namespace nexus::fbl::stm32h533 */
