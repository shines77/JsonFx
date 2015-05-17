
#ifndef _JSONFX_STREAM_STRINGINPUTSTREAM_H_
#define _JSONFX_STREAM_STRINGINPUTSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Stream/StringStreamRoot.h"

namespace JsonFx {

// Forward declaration.
template <typename CharT = JSONFX_DEFAULT_CHARTYPE>
class BasicStringInputStream;

// Define default BasicStringInputStream<CharT>.
typedef BasicStringInputStream<>  StringInputStream;

template <typename CharT>
class BasicStringInputStream : virtual public BasicStringStreamRoot<CharT>
{
public:
    typedef typename BasicStringStreamRoot<CharT>::CharType    CharType;
    typedef typename BasicStringStreamRoot<CharT>::SizeType    SizeType;

protected:
    const CharType * mReadCursor;

public:
    BasicStringInputStream(const CharType * src)
        : BasicStringStreamRoot<CharT>(src), mReadCursor(src)
    {
        jfx_iostream_trace("00 BasicStringInputStream<T>::BasicStringInputStream(const CharType * src);\n");
    }

    BasicStringInputStream(const void * src)
        : BasicStringStreamRoot<CharT>(src),
          mReadCursor(reinterpret_cast<const CharType *>(src))
    {
        jfx_iostream_trace("00 BasicStringInputStream<T>::BasicStringInputStream(const void * src);\n");
    }

    ~BasicStringInputStream() {
        jfx_iostream_trace("01 BasicStringInputStream<T>::~BasicStringInputStream();\n");
    }

    // Get properties
    CharType *       getReadCursor() const   { return const_cast<CharType *>(mReadCursor); }
    const CharType * getReadCursorC() const  { return mReadCursor; }
    void *           getReadCursorV() const  { return reinterpret_cast<void *>(getReadCursor());  }
    const void *     getReadCursorCV() const { return const_cast<const void *>(getReadCursorV()); }

    CharType *       getCurrent() const     { return getReadCursor();   }
    const CharType * getCurrentC() const    { return getReadCursorC();  }
    void *           getCurrentV() const    { return getReadCursorV();  }
    const void *     getCurrentCV() const   { return getReadCursorCV(); }

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
    bool isReadUnderflow() const { return (mReadCursor < mBegin); }
    bool isReadOverflow() const  { return (mReadCursor > getEnd());   }
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
    SizeType tell() const    { return getReadPosition(); }
    SizeType getSize() const { return getReadPosition(); }
};

}  // namespace JsonFx

// Define default StringInputStream class type
typedef JsonFx::BasicStringInputStream<JSONFX_DEFAULT_CHARTYPE>    jfxStringInputStream;

#endif  /* _JSONFX_STREAM_STRINGINPUTSTREAM_H_ */
