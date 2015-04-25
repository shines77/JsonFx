
#ifndef _JSONFX_STREAM_STRINGSTREAM_H_
#define _JSONFX_STREAM_STRINGSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "jimi/basic/stdsize.h"

#include "JsonFx/Config.h"
#include "JsonFx/Stream/StringInputStream.h"
#include "JsonFx/Stream/StringOutputStream.h"

namespace JsonFx {

// Forward declaration.
template <typename CharT = JSONFX_DEFAULT_CHARTYPE>
class BasicStringStream;

// Define default BasicStringStream<CharT>.
typedef BasicStringStream<>  StringStream;

template <typename CharT>
class BasicStringStream : public BasicStringInputStream<CharT>,
                          public BasicStringOutputStream<CharT>
{
public:
    typedef typename BasicStringInputStream<CharT>::CharType    CharType;
    typedef typename BasicStringInputStream<CharT>::SizeType    SizeType;

public:
    static const size_t kMaxMemoryAddress = BasicStringInputStream<CharT>::kMaxMemoryAddress;

protected:
    //

public:
    BasicStringStream(const CharType * src)
        : BasicStringInputStream<CharT>(src),
          BasicStringOutputStream<CharT>(src),
          BasicStringStreamRoot<CharT>(src)
    {
        jfx_iostream_trace("00 BasicStringStream<T>::BasicStringStream(const CharType * src);\n");
    }

    BasicStringStream(const CharType * src, SizeType size)
        : BasicStringInputStream<CharT>(src, size),
          BasicStringOutputStream<CharT>(src, size),
          BasicStringStreamRoot<CharT>(src, size)
    {
        jfx_iostream_trace("00 BasicStringStream<T>::BasicStringStream(const CharType * src, SizeType size);\n");
    }

    template<SizeType N>
    BasicStringStream(const CharType (&src)[N])
        : BasicStringInputStream<CharT>(src, N - 1),
          BasicStringOutputStream<CharT>(src, N - 1),
          BasicStringStreamRoot<CharT>(src, N - 1)
    {
        jfx_iostream_trace("00 BasicStringStream<T>::BasicStringStream(const CharType (&src)[N]);\n");
    }

    BasicStringStream(const void * src)
        : BasicStringInputStream<CharT>(src),
          BasicStringOutputStream<CharT>(src),
          BasicStringStreamRoot<CharT>(src)
    {
        jfx_iostream_trace("00 BasicStringStream<T>::BasicStringStream(const void * src);\n");
    }

    BasicStringStream(const void * src, SizeType size)
        : BasicStringInputStream<CharT>(src, size),
          BasicStringOutputStream<CharT>(src, size),
          BasicStringStreamRoot<CharT>(src, size)
    {
        jfx_iostream_trace("00 BasicStringStream<T>::BasicStringStream(const void * src, SizeType size);\n");
    }

    ~BasicStringStream() {
        jfx_iostream_trace("01 BasicStringStream<T>::~BasicStringStream();\n");
    }
};

}  // namespace JsonFx

// Define default StringStream class type
typedef JsonFx::BasicStringStream<JSONFX_DEFAULT_CHARTYPE>    jfxStringStream;

#endif  /* _JSONFX_STREAM_STRINGSTREAM_H_ */
