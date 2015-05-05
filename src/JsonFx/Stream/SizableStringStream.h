
#ifndef _JSONFX_STREAM_SIZABLESTRINGSTREAM_H_
#define _JSONFX_STREAM_SIZABLESTRINGSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Stream/SizableStringInputStream.h"
#include "JsonFx/Stream/SizableStringOutputStream.h"

namespace JsonFx {

// Forward declaration.
template <typename CharT = JSONFX_DEFAULT_CHARTYPE>
class BasicSizableStringStream;

// Define default BasicSizableStringStream<CharT>.
typedef BasicSizableStringStream<>  SizableStringStream;

template <typename CharT>
class BasicSizableStringStream : public BasicSizableStringInputStream<CharT>,
                                 public BasicSizableStringOutputStream<CharT>
{
public:
    typedef typename BasicSizableStringInputStream<CharT>::CharType     CharType;
    typedef typename BasicSizableStringOutputStream<CharT>::SizeType    SizeType;

public:
    BasicSizableStringStream(const CharType * src)
        : BasicSizableStringInputStream<CharT>(src),
          BasicSizableStringOutputStream<CharT>(src),
          BasicSizableStringStreamRoot<CharT>(src)
    {
        jfx_iostream_trace("00 BasicSizableStringStream<T>::BasicSizableStringStream(const CharType * src);\n");
    }

    BasicSizableStringStream(const CharType * src, SizeType size)
        : BasicSizableStringInputStream<CharT>(src, size),
          BasicSizableStringOutputStream<CharT>(src, size),
          BasicSizableStringStreamRoot<CharT>(src, size)
    {
        jfx_iostream_trace("00 BasicSizableStringStream<T>::BasicSizableStringStream(const CharType * src, SizeType size);\n");
    }

    template<SizeType N>
    BasicSizableStringStream(const CharType (&src)[N])
        : BasicSizableStringInputStream<CharT>(src, N - 1),
          BasicSizableStringOutputStream<CharT>(src, N - 1),
          BasicSizableStringStreamRoot<CharT>(src, N - 1)
    {
        jfx_iostream_trace("00 BasicSizableStringStream<T>::BasicSizableStringStream(const CharType (&src)[N]);\n");
    }

    BasicSizableStringStream(const void * src)
        : BasicSizableStringInputStream<CharT>(src),
          BasicSizableStringOutputStream<CharT>(src),
          BasicSizableStringStreamRoot<CharT>(src)
    {
        jfx_iostream_trace("00 BasicSizableStringStream<T>::BasicSizableStringStream(const void * src);\n");
    }

    BasicSizableStringStream(const void * src, SizeType size)
        : BasicSizableStringInputStream<CharT>(src, size),
          BasicSizableStringOutputStream<CharT>(src, size),
          BasicSizableStringStreamRoot<CharT>(src, size)
    {
        jfx_iostream_trace("00 BasicSizableStringStream<T>::BasicSizableStringStream(const void * src, SizeType size);\n");
    }

    ~BasicSizableStringStream() {
        jfx_iostream_trace("01 BasicSizableStringStream<T>::~BasicSizableStringStream();\n");
    }
};

}  // namespace JsonFx

// Define default SizableStringStream class type
typedef JsonFx::BasicSizableStringStream<JSONFX_DEFAULT_CHARTYPE>    jfxSizableStringStream;

#endif  /* _JSONFX_STREAM_SIZABLESTRINGSTREAM_H_ */
