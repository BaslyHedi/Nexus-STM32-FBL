/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      drivers/stm32f407/startup/startup.cpp
 * @brief     Vector table and reset sequence of the STM32F407 FBL.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: SW-03, SW-07
 *
 * The FBL is stored in the flash and executes from the SRAM (SW-07). The table below and the code
 * in .boot_text are the only parts used from the flash. The link-time layout and the steps are in
 * linker/README.md and linker/fbl.ld. ST's startup_stm32f407xx.s is not used: it copies only .data
 * and calls SystemInit from the flash. It was the reference for the order of the 98 vectors.
 */

#include <cstddef>
#include <cstdint>

#include "stm32f4xx.h"

/* Symbols of linker/fbl.ld. */
extern "C" {
extern std::uint32_t __isr_vector_load[];
extern std::uint32_t __vector_ram[];
extern std::uint32_t __vector_ram_end[];
extern std::uint32_t __ram_image_load[];
extern std::uint32_t __ram_image_start[];
extern std::uint32_t __ram_image_end[];
extern std::uint32_t __bss_start[];
extern std::uint32_t __bss_end[];
extern std::uint32_t __stack_top[];

using Constructor = void (*)();
extern Constructor __preinit_array_start[];
extern Constructor __preinit_array_end[];
extern Constructor __init_array_start[];
extern Constructor __init_array_end[];

/* main is in the SRAM. Calling main is ill-formed C++, so the call goes through an asm label. */
int callMain() __asm__("main");

[[noreturn]] void Reset_Handler();
[[noreturn]] void Reset_Main();
void Default_Handler();
void Fault_Handler() noexcept;
}

namespace {

/* The first word is the initial stack pointer, the others are handler addresses. */
using Handler = void (*)();

union VectorEntry {
    Handler handler;
    const void* address;
    constexpr VectorEntry(Handler h) noexcept : handler(h) {}
    constexpr VectorEntry(const void* a) noexcept : address(a) {}
};

}  /* namespace */

/*
 * Every handler a driver may define. A handler that is not defined is the weak alias of Default_Handler.
 * A macro because the same list declares the aliases and fills the table: written twice by hand, the two would drift.
 * Reserved79 is the vector the F407 does not use (CRYP on the F417).
 * clang-format off
 */
#define NEXUS_IRQ_HANDLERS(X) \
    X(WWDG) X(PVD) X(TAMP_STAMP) X(RTC_WKUP) X(FLASH) X(RCC) \
    X(EXTI0) X(EXTI1) X(EXTI2) X(EXTI3) X(EXTI4) \
    X(DMA1_Stream0) X(DMA1_Stream1) X(DMA1_Stream2) X(DMA1_Stream3) \
    X(DMA1_Stream4) X(DMA1_Stream5) X(DMA1_Stream6) \
    X(ADC) X(CAN1_TX) X(CAN1_RX0) X(CAN1_RX1) X(CAN1_SCE) X(EXTI9_5) \
    X(TIM1_BRK_TIM9) X(TIM1_UP_TIM10) X(TIM1_TRG_COM_TIM11) X(TIM1_CC) \
    X(TIM2) X(TIM3) X(TIM4) \
    X(I2C1_EV) X(I2C1_ER) X(I2C2_EV) X(I2C2_ER) X(SPI1) X(SPI2) \
    X(USART1) X(USART2) X(USART3) X(EXTI15_10) X(RTC_Alarm) X(OTG_FS_WKUP) \
    X(TIM8_BRK_TIM12) X(TIM8_UP_TIM13) X(TIM8_TRG_COM_TIM14) X(TIM8_CC) \
    X(DMA1_Stream7) X(FSMC) X(SDIO) X(TIM5) X(SPI3) X(UART4) X(UART5) \
    X(TIM6_DAC) X(TIM7) \
    X(DMA2_Stream0) X(DMA2_Stream1) X(DMA2_Stream2) X(DMA2_Stream3) X(DMA2_Stream4) \
    X(ETH) X(ETH_WKUP) X(CAN2_TX) X(CAN2_RX0) X(CAN2_RX1) X(CAN2_SCE) X(OTG_FS) \
    X(DMA2_Stream5) X(DMA2_Stream6) X(DMA2_Stream7) X(USART6) X(I2C3_EV) X(I2C3_ER) \
    X(OTG_HS_EP1_OUT) X(OTG_HS_EP1_IN) X(OTG_HS_WKUP) X(OTG_HS) X(DCMI) \
    X(Reserved79) X(HASH_RNG) X(FPU)
/* clang-format on */

#define NEXUS_DECLARE_WEAK_IRQ(name) \
    extern "C" void name##_IRQHandler() __attribute__((weak, alias("Default_Handler")));

NEXUS_IRQ_HANDLERS(NEXUS_DECLARE_WEAK_IRQ)

/*
 * System handlers (PM0214 table 12). Reset has its own definition below.
 * NMI and the faults default to Fault_Handler, which is in the flash: until the copy is done,
 * the table is the one in the flash and a handler in the SRAM would be unprogrammed memory.
 */
extern "C" {
void NMI_Handler() noexcept __attribute__((weak, alias("Fault_Handler")));
void HardFault_Handler() noexcept __attribute__((weak, alias("Fault_Handler")));
void MemManage_Handler() noexcept __attribute__((weak, alias("Fault_Handler")));
void BusFault_Handler() noexcept __attribute__((weak, alias("Fault_Handler")));
void UsageFault_Handler() noexcept __attribute__((weak, alias("Fault_Handler")));
void SVC_Handler() __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler() __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler() __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler() __attribute__((weak, alias("Default_Handler")));
}

namespace {

#define NEXUS_VECTOR_ENTRY(name) VectorEntry(name##_IRQHandler),

constexpr std::size_t kVectorCount = 98;  /* 16 system + 82 interrupts (RM0090 Rev 22) */

/* fbl.ld puts .isr_vector at the start of the flash and keeps it. */
[[gnu::section(".isr_vector"), gnu::used]] const VectorEntry kVectorTable[] = {
    VectorEntry(static_cast<const void*>(__stack_top)),  /*  0 initial stack pointer */
    VectorEntry(Reset_Handler),                          /*  1 */
    VectorEntry(NMI_Handler),                            /*  2 */
    VectorEntry(HardFault_Handler),                      /*  3 */
    VectorEntry(MemManage_Handler),                      /*  4 */
    VectorEntry(BusFault_Handler),                       /*  5 */
    VectorEntry(UsageFault_Handler),                     /*  6 */
    VectorEntry(Handler{nullptr}),                       /*  7 reserved */
    VectorEntry(Handler{nullptr}),                       /*  8 reserved */
    VectorEntry(Handler{nullptr}),                       /*  9 reserved */
    VectorEntry(Handler{nullptr}),                       /* 10 reserved */
    VectorEntry(SVC_Handler),                            /* 11 */
    VectorEntry(DebugMon_Handler),                       /* 12 */
    VectorEntry(Handler{nullptr}),                       /* 13 reserved */
    VectorEntry(PendSV_Handler),                         /* 14 */
    VectorEntry(SysTick_Handler),                        /* 15 */
    NEXUS_IRQ_HANDLERS(NEXUS_VECTOR_ENTRY)               /* 16 to 97 */
};

static_assert(sizeof(kVectorTable) == kVectorCount * sizeof(VectorEntry), "the F407 has 98 vectors");
static_assert(sizeof(VectorEntry) == 4U, "a vector is one 32-bit word");

/*
 * Copy and clear loops are written out. They are inlined into Reset_Main (.boot_text): a helper
 * outside it would land in .ram_image, which is not copied yet.
 */
[[gnu::always_inline]] inline void copyWords(std::uint32_t* destination, const std::uint32_t* source,
                                             const std::uint32_t* destinationEnd) noexcept
{
    while (destination < destinationEnd) {
        *destination = *source;
        ++destination;
        ++source;
    }
}

[[gnu::always_inline]] inline void clearWords(std::uint32_t* begin, const std::uint32_t* end) noexcept
{
    while (begin < end) {
        *begin = 0U;
        ++begin;
    }
}

[[gnu::always_inline]] inline void runConstructors(Constructor* begin, Constructor* end) noexcept
{
    for (; begin < end; ++begin) {
        (*begin)();
    }
}

}  /* namespace */

extern "C" {

/* Reached by any interrupt without a handler. It is in .text, so in the SRAM at run time (SW-07). */
void Default_Handler()
{
    for (;;) {
    }
}

/*
 * NMI and the faults that no driver handles. In .boot_text, so it is valid from the first
 * instruction after reset. After the jump to main it is still the handler of a fault nobody handles,
 * and a loop is all there is to do then; a flash erase in progress only delays that loop.
 */
[[gnu::section(".boot_text")]] void Fault_Handler() noexcept
{
    for (;;) {
    }
}

/*
 * The core enters here at reset with MSP already loaded from word 0 of the table. The stack pointer
 * is set again so a debugger that only sets the PC to Reset_Handler gets the same start. Naked:
 * no prologue may use the stack before it is set.
 */
[[noreturn, gnu::naked, gnu::section(".boot_text")]] void Reset_Handler()
{
    asm volatile(
        "ldr r0, =__stack_top\n\t"
        "msr msp, r0\n\t"
        "bl Reset_Main\n\t"
        "b .\n\t"
        ".ltorg\n\t");
}

/*
 * Everything here runs from the flash and uses nothing from .ram_image until the copy is done.
 * The call to main crosses from the flash into the SRAM, beyond the reach of a bl: the linker adds a
 * veneer, which has to stay in .boot_text (checked in the disassembly, drivers/stm32f407/startup/README.md).
 */
[[noreturn, gnu::section(".boot_text"), gnu::used]] void Reset_Main()
{
    copyWords(__ram_image_start, __ram_image_load, __ram_image_end);
    copyWords(__vector_ram, __isr_vector_load, __vector_ram_end);

    /* The copy of the table is complete before VTOR points at it (PM0214 section 4.3.10). */
    __DSB();
    SCB->VTOR = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(__vector_ram));
    __DSB();
    __ISB();

    clearWords(__bss_start, __bss_end);

    runConstructors(__preinit_array_start, __preinit_array_end);
    runConstructors(__init_array_start, __init_array_end);

    static_cast<void>(callMain());

    /* main of a bootloader does not return. If it does, stop here and not in unrelated memory. */
    for (;;) {
    }
}

}  /* extern "C" */
