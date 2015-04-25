
#ifndef _JSONFX_STREAM_STRINGSTREAMROOT_H_
#define _JSONFX_STREAM_STRINGSTREAMROOT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "jimi/basic/stdsize.h"

#include "JsonFx/Config.h"
#include "JsonFx/Stream/StreamRoot.h"

namespace JsonFx {

// Forward declaration.
template <typename CharT = JSONFX_DEFAULT_CHARTYPE>
class BasicStringStreamRoot;

// Define default BasicStringStreamRoot<CharT>.
typedef BasicStringStreamRoot<>  StringStreamRoot;

template <typename CharT>
class BasicStringStreamRoot : public BasicStreamRoot<CharT>
{
public:
    typedef typename BasicStreamRoot<CharT>::CharType    CharType;
    typedef typename BasicStreamRoot<CharT>::SizeType    SizeType;

public:
    static const size_t kMaxMemoryAddress = static_cast<size_t>(-1);

protected:
    const CharType * mEnd;
    SizeType         mSize;

public:
    BasicStringStreamRoot(const CharType * src)
        : BasicStreamRoot<CharT>(src),
          mEnd(reinterpret_cast<const CharType *>(kMaxMemoryAddress)),
          mSize(STREAM_ROUND_CHARSIZE(reinterpret_cast<const CharType *>(kMaxMemoryAddress) - src))

    {
        jfx_iostream_trace("00 BasicStringStreamRoot<T>::BasicStringStreamRoot(const CharType * src); visited.\n");
    }

    BasicStringStreamRoot(const CharType * src, SizeType size)
        : BasicStreamRoot<CharT>(src), mEnd(src + size), mSize(size)
    {
        jfx_iostream_trace("00 BasicStringStreamRoot<T>::BasicStringStreamRoot(const CharType * src, SizeType size); visited.\n");
    }

    BasicStringStreamRoot(const void * src)
        : BasicStreamRoot<CharT>(src),
          mEnd(reinterpret_cast<const CharType *>(kMaxMemoryAddress)),
          mSize(STREAM_ROUND_CHARSIZE(reinterpret_cast<const CharType *>(kMaxMemoryAddress) - src))
    {
        jfx_iostream_trace("00 BasicStringStreamRoot<T>::BasicStringStreamRoot(const CharType * src); visited.\n");
    }

    BasicStringStreamRoot(const void * src, SizeType size)
        : BasicStreamRoot<CharT>(src),
          mEnd(reinterpret_cast<const CharType *>(reinterpret_cast<const char *>(src) + STREAM_ROUND_CHARSIZE(size))),
          mSize(STREAM_GET_CHARSIZE(size))
    {
        jfx_iostream_trace("00 BasicStringStreamRoot<T>::BasicStringStreamRoot(const CharType * src, SizeType size); visited.\n");
    }

    ~BasicStringStreamRoot() {
        jfx_iostream_trace("01 BasicStringStreamRoot<T>::~BasicStringStreamRoot() visited.\n");
    }

    // Get properties
    const CharType * getBegin() const  { return mBegin; }
    const void *     getBeginV() const { return reinterpret_cast<void *>(mBegin); }

    const CharType * getEnd() const    { return mEnd;   }
    const void *     getEndV() const   { return reinterpret_cast<void *>(mEnd);   }

    bool isValid() const  { return (mBegin != NULL && mEnd != NULL); }
    bool isNormal() const { return (mBegin <= mEnd); }

    SizeType getSize() const { returen mSize; }
};

}  // namespace JsonFx

// Define default StringStreamRoot class type
typedef JsonFx::BasicStringStreamRoot<JSONFX_DEFAULT_CHARTYPE>    jfxStringStreamRoot;

#endif  /* _JSONFX_STREAM_STRINGSTREAMROOT_H_ */
