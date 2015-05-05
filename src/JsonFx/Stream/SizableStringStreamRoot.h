
#ifndef _JSONFX_STREAM_SIZABLESTRINGSTREAMROOT_H_
#define _JSONFX_STREAM_SIZABLESTRINGSTREAMROOT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Internal/String.h"
#include "JsonFx/Stream/SizableStream.h"

namespace JsonFx {

template <typename CharT>
class CharTrait {
public:
    typedef typename CharT  CharType;
    typedef typename size_t SizeType;

public:
    static SizeType StrLen(const CharType * src) {
        //
    }
};

// Forward declaration.
template <typename CharT = JSONFX_DEFAULT_CHARTYPE>
class BasicSizableStringStreamRoot;

// Define default BasicSizableStringStreamRoot<CharT>.
typedef BasicSizableStringStreamRoot<>  SizableStringStreamRoot;

template <typename CharT>
class BasicSizableStringStreamRoot : public BasicSizableStream<CharT>
{
public:
    typedef typename BasicSizableStream<CharT>::CharType    CharType;
    typedef typename BasicSizableStream<CharT>::SizeType    SizeType;

public:
    BasicSizableStringStreamRoot(const CharType * src)
        : BasicSizableStream<CharT>(src, internal::StrLen(src))
    {
        jfx_iostream_trace("00 BasicSizableStringStreamRoot<T>::BasicSizableStringStreamRoot(const CharType * src);\n");
    }

    BasicSizableStringStreamRoot(const CharType * src, SizeType size)
        : BasicSizableStream<CharT>(src, size)
    {
        jfx_iostream_trace("00 BasicSizableStringStreamRoot<T>::BasicSizableStringStreamRoot(const CharType * src, SizeType size);\n");
    }

    BasicSizableStringStreamRoot(const void * src)
        : BasicSizableStream<CharT>(src, internal::StrLen(src))
    {
        jfx_iostream_trace("00 BasicSizableStringStreamRoot<T>::BasicSizableStringStreamRoot(const void * src);\n");
    }

    BasicSizableStringStreamRoot(const void * src, SizeType size)
        : BasicSizableStream<CharT>(src, size)
    {
        jfx_iostream_trace("00 BasicSizableStringStreamRoot<T>::BasicSizableStringStreamRoot(const void * src, SizeType size);\n");
    }

    ~BasicSizableStringStreamRoot() {
        jfx_iostream_trace("01 BasicSizableStringStreamRoot<T>::~BasicSizableStringStreamRoot();\n");
    }
};

}  // namespace JsonFx

// Define default SizableStringStreamRoot class type
typedef JsonFx::BasicSizableStringStreamRoot<JSONFX_DEFAULT_CHARTYPE>    jfxSizableStringStreamRoot;

#endif  /* _JSONFX_STREAM_SIZABLESTRINGSTREAMROOT_H_ */
