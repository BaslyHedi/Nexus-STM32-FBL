/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      drivers/stm32f407/tick/tick.cpp
 * @brief     Millisecond time base from the SysTick interrupt.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: SW-07
 *
 * The handler replaces the weak alias of the start-up code. It runs from the vector table in the
 * SRAM (SW-07), which VTOR points to before main.
 */

#include "stm32f407/tick/tick.hpp"

#include "stm32f4xx.h"

namespace {

/* Written by the interrupt, read by the main loop. A 32-bit read is one LDR on the M4: atomic. */
volatile std::uint32_t milliseconds = 0U;

}  /* namespace */

/* The name the vector table uses. With C linkage, outside the namespace. */
extern "C" void SysTick_Handler();

extern "C" void SysTick_Handler()
{
    milliseconds = milliseconds + 1U;
}

namespace nexus::fbl::stm32f407 {

void Tick::init(std::uint32_t coreClockHz)
{
    /* The counter counts LOAD down to 0, so LOAD + 1 cycles make one interrupt. 24 bits. */
    SysTick->LOAD = (coreClockHz / 1000U) - 1U;
    /* Any write clears the counter and COUNTFLAG. */
    SysTick->VAL = 0U;
    /* Processor clock, interrupt on, counter on. */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk;
}

std::uint32_t Tick::now()
{
    return milliseconds;
}

}  /* namespace nexus::fbl::stm32f407 */
