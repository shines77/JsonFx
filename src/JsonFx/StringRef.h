
#ifndef _JSONFX_STRINGREF_H_
#define _JSONFX_STRINGREF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Config.h"
#include "JsonFx/Internal/String.h"

#include "jimi/basic/stdint.h"
#include "jimi/basic/assert.h"

namespace JsonFx {

template <typename CharTypeT = JSONFX_DEFAULT_CHARTYPE>
class BasicStringRef {
public:
	typedef CharTypeT   CharType;
    typedef uint32_t    SizeType;

public:
	BasicStringRef() : mData(NULL), mSize(0) { /* Do Nothing! */ }
	~BasicStringRef() { /* Do Nothing! */ }

    template<SizeType N>
    BasicStringRef(const CharType (&str)[N])
        : mData(str), mSize(N - 1) { /* Do Nothing! */ }

    explicit BasicStringRef(const CharType * str)
        : mData(str), mSize(internal::StrLen(str)) { jimi_assert(mData != NULL); }

    BasicStringRef(const CharType * str, SizeType len)
        : mData(str), mSize(len) { jimi_assert(mData != NULL); }

    //! implicit conversion to plain CharType pointer
    operator const CharType *() const { return mData; }

    const CharType * const  mData;      //!< plain CharType pointer
    const SizeType          mSize;      //!< length of the string (excluding the trailing NULL terminator)

private:
    //! Disallow copy-assignment
    BasicStringRef operator =(const BasicStringRef & src);

    //! Disallow construction from non-const array
    template<SizeType N>
    BasicStringRef(CharType (&str)[N]); /* = delete */
};

// Define default StringRef class type
typedef BasicStringRef<>     StringRef;

}  // namespace JsonFx

#endif  /* !_JSONFX_STRINGREF_H_ */
