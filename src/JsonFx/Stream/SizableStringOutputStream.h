
#ifndef _JSONFX_STREAM_SIZABLESTRINGOUTPUTSTREAM_H_
#define _JSONFX_STREAM_SIZABLESTRINGOUTPUTSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Stream/SizableStringStreamRoot.h"

namespace JsonFx {

// Forward declaration.
template <typename CharT = JSONFX_DEFAULT_CHARTYPE>
class BasicSizableStringOutputStream;

// Define default BasicSizableStringOutputStream<CharT>.
typedef BasicSizableStringOutputStream<>   SizableStringOutputStream;

template <typename CharT>
class BasicSizableStringOutputStream : virtual public BasicSizableStringStreamRoot<CharT>
{
public:
    typedef typename BasicSizableStringStreamRoot<CharT>::CharType    CharType;
    typedef typename BasicSizableStringStreamRoot<CharT>::SizeType    SizeType;

protected:
    const CharType * mWriteCursor;

public:
    BasicSizableStringOutputStream(const CharType * src)
        : BasicSizableStringStreamRoot<CharT>(src), mWriteCursor(src)
    {
        jfx_iostream_trace("00 BasicSizableStringOutputStream<T>::BasicSizableStringOutputStream(const CharType * src);\n");
    }

    BasicSizableStringOutputStream(const CharType * src, SizeType size)
        : BasicSizableStringStreamRoot<CharT>(src, size), mWriteCursor(src)
    {
        jfx_iostream_trace("00 BasicSizableStringOutputStream<T>::BasicSizableStringOutputStream(const CharType * src, SizeType size);\n");
    }

    BasicSizableStringOutputStream(const void * src)
        : BasicSizableStringStreamRoot<CharT>(src),
          mWriteCursor(reinterpret_cast<const CharType *>(src))
    {
        jfx_iostream_trace("00 BasicSizableStringOutputStream<T>::BasicSizableStringOutputStream(const void * src);\n");
    }

    BasicSizableStringOutputStream(const void * src, SizeType size)
        : BasicSizableStringStreamRoot<CharT>(src, size),
          mWriteCursor(reinterpret_cast<const CharType *>(src))
    {
        jfx_iostream_trace("00 BasicSizableStringOutputStream<T>::BasicSizableStringOutputStream(const void * src, SizeType size);\n");
    }

    ~BasicSizableStringOutputStream() {
        jfx_iostream_trace("01 BasicSizableStringOutputStream<T>::~BasicSizableStringOutputStream();\n");
    }

    // Get properties
    const CharType * getWriteCursor() const  { return mWriteCursor; }
    const void *     getWriteCursorV() const { return reinterpret_cast<void *>(mWriteCursor); }

    // Set properties
    void setWriteCursor(CharType * newWriteCursor) {
        jimi_assert(newWriteCursor != NULL);
        mWriteCursor = newWriteCursor;
    }
    void setWriteCursor(void * newWriteCursor) {
        jimi_assert(newWriteCursor != NULL);
        mWriteCursor = reinterpret_cast<CharType *>(newWriteCursor);
    }

    // Get state
    bool isWriteEof() const {
        jimi_assert(mWriteCursor != NULL);
        jimi_assert(mWriteCursor < getEnd());
        return (*mWriteCursor == static_cast<SizeType>('\0'));
    }

    // Check range
    bool isWriteUnderflow() const { return (mWriteCursor < mBegin);   }
    bool isWriteOverflow() const  { return (mWriteCursor > getEnd()); }
    bool isWriteValid() const     { return (isWriteOverflow() && isWriteUnderflow()); }

    // Write
    void put(CharType c) {
        jimi_assert(mWriteCursor != NULL);
        *mWriteCursor++ = c;
    }
    int write(CharType c) {
        jimi_assert(mWriteCursor != NULL);
        *mWriteCursor = c;
        if (!(isWriteOverflow() && isWriteUnderflow()))
            return static_cast<int>(c);
        else
            return -1;
    }

    void nextWriteCursor() { mWriteCursor++; }

    // Get position
    SizeType getWritePosition() const {
        return static_cast<size_t>(mWriteCursor - mBegin);
    }
};

}  // namespace JsonFx

// Define default SizableStringOutputStream class type
typedef JsonFx::BasicSizableStringOutputStream<JSONFX_DEFAULT_CHARTYPE> jfxSizableStringOutputStream;

#endif  /* _JSONFX_STREAM_SIZABLESTRINGOUTPUTSTREAM_H_ */
