
#ifndef _JSONFX_INTERNAL_UTILS_H_
#define _JSONFX_INTERNAL_UTILS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "JsonFx/Config.h"

#include "jimi/basic/archdef.h"
#include "jimi/basic/stdint.h"
#include "jimi/basic/assert.h"

namespace JsonFx {

namespace internal {

// If n = 0, round to 1.
static
size_t RoundToPowerOf2(size_t n) {
#if 1
    if (n != 0) {
        // ms1b
        --n;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
    #if defined(JIMI_ARCH_X64) || defined(JIMI_ARCH_IA64)
        n |= n >> 32;
    #endif
    }
    return ++n;
#else
    size_t ms1b = 1;
    while (ms1b < n) {
        ms1b <<= 1;
    }

    return ms1b;
#endif
}

}  // namespace internal

}  // namespace jimi

#endif  /* _JSONFX_INTERNAL_UTILS_H_ */
