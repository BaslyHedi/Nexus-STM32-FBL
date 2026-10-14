/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      drivers/stm32f407/leds/leds.cpp
 * @brief     The four user LEDs of the Discovery board, PD12 to PD15.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: none
 */

#include "stm32f407/leds/leds.hpp"

#include "stm32f4xx.h"

namespace nexus::fbl::stm32f407 {

namespace {

constexpr std::uint32_t firstPin = 12U;
constexpr std::uint32_t allLeds = 0xFU;

}  /* namespace */

void Leds::init()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    /* The port takes two AHB cycles to see its clock (errata ES0182). Reading back waits for it. */
    static_cast<void>(RCC->AHB1ENR);

    /* All off before the pins become outputs, so none flashes during the set-up. */
    write(0U);

    /* MODER: two bits per pin, 01 is a general-purpose output. Push-pull, low speed and no pull
     * are the reset values of OTYPER, OSPEEDR and PUPDR. */
    GPIOD->MODER = (GPIOD->MODER & ~(GPIO_MODER_MODER12 | GPIO_MODER_MODER13 |
                                     GPIO_MODER_MODER14 | GPIO_MODER_MODER15)) |
                   GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 |
                   GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0;
}

void Leds::write(std::uint32_t mask)
{
    const std::uint32_t on = mask & allLeds;
    const std::uint32_t off = ~mask & allLeds;
    /* BSRR: the low half sets pins, the high half resets them, in one write. */
    GPIOD->BSRR = (on << firstPin) | (off << (firstPin + 16U));
}

}  /* namespace nexus::fbl::stm32f407 */
