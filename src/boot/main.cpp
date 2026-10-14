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

/* With -ffreestanding main is not special to the compiler, so -Wmissing-declarations wants this. */
int main();

int main()
{
    for (;;) {
    }
}
