/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      drivers/startup/startup.cpp
 * @brief     Vector table and reset sequence of the STM32H533 FBL.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: SW-03, SW-07
 *
 * The FBL is stored in the flash and executes from the SRAM (SW-07). The table below and the code
 * in .boot_text are the only parts used from the flash. The link-time layout and the steps are in
 * linker/README.md and linker/fbl.ld. ST's startup_stm32h533xx.s is not used: it copies only .data
 * and calls SystemInit from the flash. The order of the 133 interrupts is that of IRQn_Type in
 * stm32h533xx.h, with a ReservedN handler for each number the header leaves out.
 */

#include <cstddef>
#include <cstdint>

#include "stm32h533xx.h"

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
 * One entry per interrupt number 0 to 132 (RM0481 Table 153); ReservedN is a number with no peripheral.
 * clang-format off
 */
#define NEXUS_IRQ_HANDLERS(X) \
    X(WWDG) X(PVD_AVD) X(RTC) X(RTC_S) X(TAMP) X(RAMCFG) X(FLASH) X(FLASH_S) X(GTZC) X(RCC) \
    X(RCC_S) X(EXTI0) X(EXTI1) X(EXTI2) X(EXTI3) X(EXTI4) X(EXTI5) X(EXTI6) X(EXTI7) X(EXTI8) \
    X(EXTI9) X(EXTI10) X(EXTI11) X(EXTI12) X(EXTI13) X(EXTI14) X(EXTI15) X(GPDMA1_Channel0) \
    X(GPDMA1_Channel1) X(GPDMA1_Channel2) X(GPDMA1_Channel3) X(GPDMA1_Channel4) \
    X(GPDMA1_Channel5) X(GPDMA1_Channel6) X(GPDMA1_Channel7) X(IWDG) X(SAES) X(ADC1) X(DAC1) \
    X(FDCAN1_IT0) X(FDCAN1_IT1) X(TIM1_BRK) X(TIM1_UP) X(TIM1_TRG_COM) X(TIM1_CC) X(TIM2) \
    X(TIM3) X(TIM4) X(TIM5) X(TIM6) X(TIM7) X(I2C1_EV) X(I2C1_ER) X(I2C2_EV) X(I2C2_ER) X(SPI1) \
    X(SPI2) X(SPI3) X(USART1) X(USART2) X(USART3) X(UART4) X(UART5) X(LPUART1) X(LPTIM1) \
    X(TIM8_BRK) X(TIM8_UP) X(TIM8_TRG_COM) X(TIM8_CC) X(ADC2) X(LPTIM2) X(TIM15) X(Reserved72) \
    X(Reserved73) X(USB_DRD_FS) X(CRS) X(UCPD1) X(FMC) X(OCTOSPI1) X(SDMMC1) X(I2C3_EV) \
    X(I2C3_ER) X(SPI4) X(Reserved83) X(Reserved84) X(USART6) X(Reserved86) X(Reserved87) \
    X(Reserved88) X(Reserved89) X(GPDMA2_Channel0) X(GPDMA2_Channel1) X(GPDMA2_Channel2) \
    X(GPDMA2_Channel3) X(GPDMA2_Channel4) X(GPDMA2_Channel5) X(GPDMA2_Channel6) \
    X(GPDMA2_Channel7) X(Reserved98) X(Reserved99) X(Reserved100) X(Reserved101) X(Reserved102) \
    X(FPU) X(ICACHE) X(DCACHE1) X(Reserved106) X(Reserved107) X(DCMI_PSSI) X(FDCAN2_IT0) \
    X(FDCAN2_IT1) X(Reserved111) X(Reserved112) X(DTS) X(RNG) X(OTFDEC1) X(AES) X(HASH) X(PKA) \
    X(CEC) X(TIM12) X(Reserved121) X(Reserved122) X(I3C1_EV) X(I3C1_ER) X(Reserved125) \
    X(Reserved126) X(Reserved127) X(Reserved128) X(Reserved129) X(Reserved130) X(I3C2_EV) \
    X(I3C2_ER)
/* clang-format on */

#define NEXUS_DECLARE_WEAK_IRQ(name) \
    extern "C" void name##_IRQHandler() __attribute__((weak, alias("Default_Handler")));

NEXUS_IRQ_HANDLERS(NEXUS_DECLARE_WEAK_IRQ)

/*
 * System handlers (Armv8-M exception numbers 2 to 15). Reset has its own definition below.
 * NMI and the faults default to Fault_Handler, which is in the flash: until the copy is done,
 * the table is the one in the flash and a handler in the SRAM would be unprogrammed memory.
 */
extern "C" {
void NMI_Handler() noexcept __attribute__((weak, alias("Fault_Handler")));
void HardFault_Handler() noexcept __attribute__((weak, alias("Fault_Handler")));
void MemManage_Handler() noexcept __attribute__((weak, alias("Fault_Handler")));
void BusFault_Handler() noexcept __attribute__((weak, alias("Fault_Handler")));
void UsageFault_Handler() noexcept __attribute__((weak, alias("Fault_Handler")));
void SecureFault_Handler() noexcept __attribute__((weak, alias("Fault_Handler")));
void SVC_Handler() __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler() __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler() __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler() __attribute__((weak, alias("Default_Handler")));
}

namespace {

#define NEXUS_VECTOR_ENTRY(name) VectorEntry(name##_IRQHandler),

constexpr std::size_t kVectorCount = 149;  /* 16 system + 133 interrupts (RM0481 Table 153) */

/* fbl.ld puts .isr_vector at the start of the flash and keeps it. */
[[gnu::section(".isr_vector"), gnu::used]] const VectorEntry kVectorTable[] = {
    VectorEntry(static_cast<const void*>(__stack_top)),  /*  0 initial stack pointer */
    VectorEntry(Reset_Handler),                          /*  1 */
    VectorEntry(NMI_Handler),                            /*  2 */
    VectorEntry(HardFault_Handler),                      /*  3 */
    VectorEntry(MemManage_Handler),                      /*  4 */
    VectorEntry(BusFault_Handler),                       /*  5 */
    VectorEntry(UsageFault_Handler),                     /*  6 */
    VectorEntry(SecureFault_Handler),                    /*  7 */
    VectorEntry(Handler{nullptr}),                       /*  8 reserved */
    VectorEntry(Handler{nullptr}),                       /*  9 reserved */
    VectorEntry(Handler{nullptr}),                       /* 10 reserved */
    VectorEntry(SVC_Handler),                            /* 11 */
    VectorEntry(DebugMon_Handler),                       /* 12 */
    VectorEntry(Handler{nullptr}),                       /* 13 reserved */
    VectorEntry(PendSV_Handler),                         /* 14 */
    VectorEntry(SysTick_Handler),                        /* 15 */
    NEXUS_IRQ_HANDLERS(NEXUS_VECTOR_ENTRY)               /* 16 to 148 */
};

static_assert(sizeof(kVectorTable) == kVectorCount * sizeof(VectorEntry), "the H533 has 149 vectors");
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
 * veneer, which has to stay in .boot_text (checked in the disassembly, drivers/startup/README.md).
 */
[[noreturn, gnu::section(".boot_text"), gnu::used]] void Reset_Main()
{
    copyWords(__ram_image_start, __ram_image_load, __ram_image_end);
    copyWords(__vector_ram, __isr_vector_load, __vector_ram_end);

    /* The copy of the table is complete before VTOR points at it (Armv8-M, VTOR). */
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
