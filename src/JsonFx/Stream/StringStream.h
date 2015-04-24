
#ifndef _JSONFX_STREAM_STRINGSTREAM_H_
#define _JSONFX_STREAM_STRINGSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "jimi/basic/stdsize.h"
#include "JsonFx/Config.h"

namespace JsonFx {

// Forward declaration.
template <typename T = JSONFX_DEFAULT_CHARTYPE>
class BasicStringStream;

// Define default BasicStringStream<T>.
typedef BasicStringStream<>  StringStream;

template <typename T>
class BasicStringStream
{
public:
    typedef T       CharType;
    typedef size_t  SizeType;

public:
    static const size_t kMaxMemoryAddress = static_cast<size_t>(-1);

private:
    const CharType *    mReadCursor;
    const CharType *    mWriteCursor;
    const CharType *    mBegin;
    const CharType *    mEnd;
    SizeType            mSize;

public:
    BasicStringStream(const CharType * src)
        : mReadCursor(src), mWriteCursor(NULL), mBegin(src),
          mEnd(reinterpret_cast<const CharType *>(kMaxMemoryAddress)),
          mSize(reinterpret_cast<const CharType *>(kMaxMemoryAddress) - src)
    {
        jfx_iostream_trace("00 BasicStringStream<T>::BasicStringStream() visited.\n");
    }

    BasicStringStream(const CharType * src, SizeType size)
        : mReadCursor(src), mWriteCursor(NULL), mBegin(src), mEnd(src + size),  mSize(size)
    {
        jfx_iostream_trace("00 BasicStringStream<T>::BasicStringStream() visited.\n");
    }

    ~BasicStringStream() {
        jfx_iostream_trace("00 BasicStringStream<T>::~BasicStringStream() visited.\n");
    }

    // Get properties
    const CharType * getBegin()    { return mBegin; }
    const void *     getBeginV()   { return reinterpret_cast<void *>(mBegin); }

    const CharType * getCurrent()  { return mReadCursor; }
    const void *     getCurrentV() { return reinterpret_cast<void *>(mReadCursor); }

    // Set properties
    void setCurrent(CharType * newCurrent) {
        jimi_assert(newCurrent != NULL);
        mReadCursor = newCurrent;
    }
    void setCurrent(void * newCurrent) {
        jimi_assert(newCurrent != NULL);
        mReadCursor = reinterpret_cast<CharType *>(newCurrent);
    }

    // Get state
    bool isEof() {
        jimi_assert(mReadCursor <= mEnd);
        return (*mReadCursor == static_cast<SizeType>('\0'));
    }

    bool underflow() { return (mReadCursor < mBegin); }
    bool overflow()  { return (mReadCursor > mEnd);   }
    bool isValid()   { return (overflow() && underflow()); }

    // Read
    CharType get()  { return *mReadCursor;   }
    CharType peek() { return *mReadCursor;   }
    CharType take() { return *mReadCursor++; }

    // Write
    void put(CharType c) {
        jimi_assert(mWriteCursor != NULL);
        *mWriteCursor++ = c;
    }
    int write(CharType c) {
        jimi_assert(mWriteCursor != NULL);
        *mWriteCursor = c;
        if (!(overflow() && underflow()))
            return static_cast<int>(c);
        else
            return -1;
    }

    // Get position
    SizeType tell() {
        return static_cast<size_t>(mReadCursor - mBegin);
    }
};

}  // namespace JsonFx

// Define default StringStream class type
typedef JsonFx::BasicStringStream<JSONFX_DEFAULT_CHARTYPE>    jfxStringStream;

#endif  /* _JSONFX_STREAM_STRINGSTREAM_H_ */
