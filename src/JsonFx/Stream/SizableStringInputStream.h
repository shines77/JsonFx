
#ifndef _JSONFX_STREAM_SIZABLESTRINGINPUTSTREAM_H_
#define _JSONFX_STREAM_SIZABLESTRINGINPUTSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Stream/SizableStringStreamRoot.h"

namespace JsonFx {

// Forward declaration.
template <typename CharT = JSONFX_DEFAULT_CHARTYPE>
class BasicSizableStringInputStream;

// Define default BasicSizableStringInputStream<CharT>.
typedef BasicSizableStringInputStream<>  SizableStringInputStream;

template <typename CharT>
class BasicSizableStringInputStream : virtual public BasicSizableStringStreamRoot<CharT>
{
public:
    typedef typename BasicSizableStringStreamRoot<CharT>::CharType    CharType;
    typedef typename BasicSizableStringStreamRoot<CharT>::SizeType    SizeType;

protected:
    const CharType * mReadCursor;

public:
    BasicSizableStringInputStream(const CharType * src)
        : BasicSizableStringStreamRoot<CharT>(src), mReadCursor(src)
    {
        jfx_iostream_trace("00 BasicSizableStringInputStream<T>::BasicSizableStringInputStream(const CharType * src);\n");
    }

    BasicSizableStringInputStream(const CharType * src, SizeType size)
        : BasicSizableStringStreamRoot<CharT>(src, size), mReadCursor(src)
    {
        jfx_iostream_trace("00 BasicSizableStringInputStream<T>::BasicSizableStringInputStream(const CharType * src, SizeType size);\n");
    }

    BasicSizableStringInputStream(const void * src)
        : BasicSizableStringStreamRoot<CharT>(src),
          mReadCursor(reinterpret_cast<const CharType *>(src))
    {
        jfx_iostream_trace("00 BasicSizableStringInputStream<T>::BasicSizableStringInputStream(const void * src);\n");
    }

    BasicSizableStringInputStream(const void * src, SizeType size)
        : BasicSizableStringStreamRoot<CharT>(src, size),
          mReadCursor(reinterpret_cast<const CharType *>(src))
    {
        jfx_iostream_trace("00 BasicSizableStringInputStream<T>::BasicSizableStringInputStream(const void * src, SizeType size);\n");
    }

    ~BasicSizableStringInputStream() {
        jfx_iostream_trace("01 BasicSizableStringInputStream<T>::~BasicSizableStringInputStream();\n");
    }

    // Get properties
    const CharType * getReadCursor() const  { return mReadCursor; }
    const void *     getReadCursorV() const { return reinterpret_cast<void *>(mReadCursor); }

    const CharType * getCurrent() const     { return getReadCursor();  }
    const void *     getCurrentV() const    { return getReadCursorV(); }

    // Set properties
    void setReadCursor(CharType * newReadCursor) {
        jimi_assert(newReadCursor != NULL);
        mReadCursor = newReadCursor;
    }
    void setReadCursor(void * newReadCursor) {
        jimi_assert(newReadCursor != NULL);
        mReadCursor = reinterpret_cast<CharType *>(newReadCursor);
    }

    void setCurrent(CharType * newCurrent) { return setReadCursor(newCurrent); }
    void setCurrent(void * newCurrent)     { return setReadCursor(newCurrent); }

    // Get state
    bool isReadEof() const {
        jimi_assert(mReadCursor != NULL);
        jimi_assert(mReadCursor < getEnd());
        return (*mReadCursor == static_cast<SizeType>('\0'));
    }
    bool isEof() const { return isReadEof(); }

    // Check range
    bool isReadUnderflow() const { return (mReadCursor < mBegin);   }
    bool isReadOverflow() const  { return (mReadCursor > getEnd()); }
    bool isReadValid() const     { return (isReadOverflow() && isReadUnderflow()); }

    bool underflow() const { return isReadUnderflow(); }
    bool overflow() const  { return isReadOverflow();  }
    bool isValid() const   { return (overflow() && underflow()); }

    // Read
    CharType get() const  { return *mReadCursor;   }
    CharType peek() const { return *mReadCursor;   }
    CharType take()       { return *mReadCursor++; }

    void nextReadCursor() { mReadCursor++;           }
    void next()           { return nextReadCursor(); }

    // Get position
    SizeType getReadPosition() const {
        return static_cast<size_t>(mReadCursor - mBegin);
    }
    SizeType tell() const { return getReadPosition(); }
};

}  // namespace JsonFx

// Define default SizableStringInputStream class type
typedef JsonFx::BasicSizableStringInputStream<JSONFX_DEFAULT_CHARTYPE>      jfxSizableStringInputStream;

#endif  /* _JSONFX_STREAM_SIZABLESTRINGINPUTSTREAM_H_ */
