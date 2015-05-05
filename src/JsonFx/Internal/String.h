
#ifndef _JSONFX_INTERNAL_STRING_H_
#define _JSONFX_INTERNAL_STRING_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "JsonFx/Config.h"

#include "jimi/basic/stdint.h"
#include "jimi/basic/assert.h"

namespace JsonFx {

namespace internal {

template <typename CharType>
size_t StrLen(const CharType * str) {
    const CharType * orig = str;
    while (*str++ != CharType('\0')) {
        // Do nothing!
    }
    return reinterpret_cast<size_t>(str - orig);
}

template <>
size_t StrLen(const char * str) {
    return ::strlen(str);
}

template <>
size_t StrLen(const unsigned char * str) {
    return ::strlen((const char *)str);
}

}  // namespace internal

}  // namespace jimi

#endif  /* _JSONFX_INTERNAL_STRING_H_ */
