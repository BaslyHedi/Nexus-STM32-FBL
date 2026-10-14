/*
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2026 Hedi Basly
 */

/**
 * @file      drivers/runtime/memory.cpp
 * @brief     The four memory functions GCC may call in a freestanding image.
 *
 * Element:   Nexus-Physical-01-04 (bootloader)
 * Decisions: SW-03
 *
 * GCC emits calls to memcpy, memmove, memset and memcmp on its own, to copy or clear an object,
 * and requires a freestanding environment to provide them. The image links no C library (SW-03),
 * so they are here. Byte by byte: small, and fast enough for the FBL.
 *
 * Built with -fno-tree-loop-distribute-patterns: without it GCC would turn the loop of memset
 * into a call to memset, and the function would call itself until the stack overflows.
 */

#include <cstddef>

extern "C" {
void* memcpy(void* dest, const void* src, std::size_t count);
void* memmove(void* dest, const void* src, std::size_t count);
void* memset(void* dest, int value, std::size_t count);
int memcmp(const void* lhs, const void* rhs, std::size_t count);
}

extern "C" void* memcpy(void* dest, const void* src, std::size_t count)
{
    auto* const d = static_cast<unsigned char*>(dest);
    const auto* const s = static_cast<const unsigned char*>(src);
    for (std::size_t i = 0U; i < count; ++i) {
        d[i] = s[i];
    }
    return dest;
}

extern "C" void* memmove(void* dest, const void* src, std::size_t count)
{
    auto* const d = static_cast<unsigned char*>(dest);
    const auto* const s = static_cast<const unsigned char*>(src);
    if (d < s) {
        for (std::size_t i = 0U; i < count; ++i) {
            d[i] = s[i];
        }
    } else {
        /* dest above src: copy from the end, so an overlap is read before it is overwritten. */
        for (std::size_t i = count; i > 0U; --i) {
            d[i - 1U] = s[i - 1U];
        }
    }
    return dest;
}

extern "C" void* memset(void* dest, int value, std::size_t count)
{
    auto* const d = static_cast<unsigned char*>(dest);
    const auto byte = static_cast<unsigned char>(value);
    for (std::size_t i = 0U; i < count; ++i) {
        d[i] = byte;
    }
    return dest;
}

extern "C" int memcmp(const void* lhs, const void* rhs, std::size_t count)
{
    const auto* const a = static_cast<const unsigned char*>(lhs);
    const auto* const b = static_cast<const unsigned char*>(rhs);
    for (std::size_t i = 0U; i < count; ++i) {
        if (a[i] != b[i]) {
            return static_cast<int>(a[i]) - static_cast<int>(b[i]);
        }
    }
    return 0;
}
