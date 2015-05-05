
#ifndef _JSONFX_STREAM_FIXEDSTREAM_H_
#define _JSONFX_STREAM_FIXEDSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Stream/StreamRoot.h"

namespace JsonFx {

// Forward declaration.
template <typename CharT = JSONFX_DEFAULT_CHARTYPE>
class BasicFixedStream;

// Define default BasicFixedStream<CharT>.
typedef BasicFixedStream<>  FixedStream;

template <size_t _Size, typename CharT>
class BasicFixedStream : public BasicStreamRoot<CharT>
{
public:
    typedef typename BasicStreamRoot<CharT>::CharType    CharType;
    typedef typename BasicStreamRoot<CharT>::SizeType    SizeType;

public:
    static const size_t kSize = _Size;

protected:
    const CharType * mEnd;

public:
    BasicFixedStream(const CharType * src) : BasicStreamRoot<CharT>(src), mEnd(src + kSize),
    {
        jfx_iostream_trace("00 BasicFixedStream<T>::BasicFixedStream(const CharType * src);\n");
    }

    BasicFixedStream(const void * src) : BasicStreamRoot<CharT>(src),
        mEnd(reinterpret_cast<const CharType *>(reinterpret_cast<const char *>(src) + kSize))
    {
        jfx_iostream_trace("00 BasicFixedStream<T>::BasicFixedStream(const void * src);\n");
    }

    template<SizeType N>
    BasicFixedStream(const CharType (&src)[N]) : BasicStreamRoot<CharT>(src), mEnd(src + N)
    {
        jfx_iostream_trace("00 BasicStringStream<T>::BasicStringStream(const CharType (&src)[N]);\n");
    }

    ~BasicFixedStream() {
        jfx_iostream_trace("01 BasicFixedStream<T>::~BasicFixedStream();\n");
    }

    // Get properties
    const CharType * getEnd() const    { return mEnd; }
    const void *     getEndV() const   { return reinterpret_cast<void *>(mEnd); }

    bool isValid() const  { return (mBegin != NULL); }
    bool isNormal() const { return true; }

    SizeType getSize() const { return kSize; }
};

}  // namespace JsonFx

// Define default FixedStream class type
typedef JsonFx::BasicFixedStream<JSONFX_DEFAULT_CHARTYPE>    jfxFixedStream;

#endif  /* _JSONFX_STREAM_FIXEDSTREAM_H_ */
