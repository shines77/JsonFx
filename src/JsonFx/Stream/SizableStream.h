
#ifndef _JSONFX_STREAM_SIZABLESTREAM_H_
#define _JSONFX_STREAM_SIZABLESTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Stream/StreamRoot.h"

namespace JsonFx {

// Forward declaration.
template <typename CharT = JSONFX_DEFAULT_CHARTYPE>
class BasicSizableStream;

// Define default BasicSizableStream<CharT>.
typedef BasicSizableStream<>  SizableStream;

template <typename CharT>
class BasicSizableStream : public BasicStreamRoot<CharT>
{
public:
    typedef typename BasicStreamRoot<CharT>::CharType    CharType;
    typedef typename BasicStreamRoot<CharT>::SizeType    SizeType;

protected:
    const CharType * mEnd;
    SizeType         mSize;

public:
    BasicSizableStream(const CharType * src, SizeType size)
        : BasicStreamRoot<CharT>(src), mEnd(src + size), mSize(size)
    {
        jfx_iostream_trace("00 BasicSizableStream<T>::BasicSizableStream(const CharType * src, SizeType size);\n");
    }

    BasicSizableStream(const void * src, SizeType size)
        : BasicStreamRoot<CharT>(src),
          mEnd(reinterpret_cast<const CharType *>(reinterpret_cast<const char *>(src) + STREAM_ROUND_CHARSIZE(size))),
          mSize(STREAM_GET_CHARSIZE(size))
    {
        jfx_iostream_trace("00 BasicSizableStream<T>::BasicSizableStream(const void * src, SizeType size);\n");
    }

    ~BasicSizableStream() {
        jfx_iostream_trace("01 BasicSizableStream<T>::~BasicSizableStream();\n");
    }

    // Get properties
    const CharType * getEnd() const    { return mEnd; }
    const void *     getEndV() const   { return reinterpret_cast<void *>(mEnd); }

    bool isValid() const  { return (mBegin != NULL && mEnd != NULL); }
    bool isNormal() const { return (mBegin <= mEnd); }

    SizeType getSize() const { return mSize; }
};

}  // namespace JsonFx

// Define default SizableStream class type
typedef JsonFx::BasicSizableStream<JSONFX_DEFAULT_CHARTYPE>    jfxSizableStream;

#endif  /* _JSONFX_STREAM_SIZABLESTREAM_H_ */
