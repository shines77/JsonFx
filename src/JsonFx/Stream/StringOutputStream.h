
#ifndef _JSONFX_STREAM_STRINGOUTPUTSTREAM_H_
#define _JSONFX_STREAM_STRINGOUTPUTSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Stream/StringStreamRoot.h"

namespace JsonFx {

// Forward declaration.
template <typename CharT = JSONFX_DEFAULT_CHARTYPE>
class BasicStringOutputStream;

// Define default BasicStringOutputStream<CharT>.
typedef BasicStringOutputStream<>   StringOutputStream;

// Forward declaration.
template <typename CharT = JSONFX_DEFAULT_CHARTYPE>
class BasicStringOutputStreamRoot;

// Define default BasicStringOutputStreamRoot<CharT>.
typedef BasicStringOutputStreamRoot<>   StringOutputStreamRoot;

template <typename CharT>
class BasicStringOutputStreamRoot
{
public:
    typedef typename CharT  CharType;

    BasicStringOutputStreamRoot(const CharType * src)
        : mWriteCursor(const_cast<CharType *>(src))
    {
        jfx_iostream_trace("00 BasicStringOutputStreamRoot<T>::BasicStringOutputStreamRoot(const CharType * src);\n");
    }

    BasicStringOutputStreamRoot(const void * src)
        : mWriteCursor(reinterpret_cast<CharType *>(const_cast<void *>(src)))
    {
        jfx_iostream_trace("00 BasicStringOutputStreamRoot<T>::BasicStringOutputStreamRoot(const void * src);\n");
    }

    ~BasicStringOutputStreamRoot() {
        jfx_iostream_trace("01 BasicStringOutputStreamRoot<T>::~BasicStringOutputStreamRoot();\n");
    }

protected:
    CharType * mWriteCursor;
};

template <typename CharT>
#if defined(STRING_STREAM_DERIVED_USE_ROOTCLASS) && (STRING_STREAM_DERIVED_USE_ROOTCLASS != 0)
class BasicStringOutputStream :         public BasicStringOutputStreamRoot<CharT>,
                                virtual public BasicStringStreamRoot<CharT>
#else
class BasicStringOutputStream : virtual public BasicStringStreamRoot<CharT>
#endif
{
public:
    typedef typename BasicStringStreamRoot<CharT>::CharType    CharType;
    typedef typename BasicStringStreamRoot<CharT>::SizeType    SizeType;

#if !defined(STRING_STREAM_DERIVED_USE_ROOTCLASS) || (STRING_STREAM_DERIVED_USE_ROOTCLASS == 0)
protected:
    CharType * mWriteCursor;
#endif

public:
#if defined(STRING_STREAM_DERIVED_USE_ROOTCLASS) && (STRING_STREAM_DERIVED_USE_ROOTCLASS != 0)
    BasicStringOutputStream(const CharType * src)
        : BasicStringOutputStreamRoot<CharT>(src), BasicStringStreamRoot<CharT>(src)
    {
        jfx_iostream_trace("00 BasicStringOutputStream<T>::BasicStringOutputStream(const CharType * src);\n");
    }

    BasicStringOutputStream(const void * src)
        : BasicStringOutputStreamRoot<CharT>(src), BasicStringStreamRoot<CharT>(src),
    {
        jfx_iostream_trace("00 BasicStringOutputStream<T>::BasicStringOutputStream(const void * src);\n");
    }
#else
    BasicStringOutputStream(const CharType * src)
        : BasicStringStreamRoot<CharT>(src), mWriteCursor(const_cast<CharType *>(src))
    {
        jfx_iostream_trace("00 BasicStringOutputStream<T>::BasicStringOutputStream(const CharType * src);\n");
    }

    BasicStringOutputStream(const void * src)
        : BasicStringStreamRoot<CharT>(src),
          mWriteCursor(reinterpret_cast<CharType *>(const_cast<void *>(src)))
    {
        jfx_iostream_trace("00 BasicStringOutputStream<T>::BasicStringOutputStream(const void * src);\n");
    }
#endif

    ~BasicStringOutputStream() {
        jfx_iostream_trace("01 BasicStringOutputStream<T>::~BasicStringOutputStream();\n");
    }

    // Get properties
    CharType *       getWriteCursor() const   { return mWriteCursor; }
    const CharType * getWriteCursorC() const  { return const_cast<const CharType *>(mWriteCursor); }
    void *           getWriteCursorV() const  { return reinterpret_cast<void *>(mWriteCursor);     }
    const void *     getWriteCursorCV() const { return const_cast<const void *>(getWriteCursor()); }

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
    bool isEofW() const { return isWriteEof(); }

    // Check range
    bool isWriteUnderflow() const { return (mWriteCursor < mBegin);   }
    bool isWriteOverflow() const  { return (mWriteCursor > getEnd()); }
    bool isWriteValid() const     { return (isWriteOverflow() && isWriteUnderflow()); }

    bool isValidW() const  { return (isWriteOverflow() && isWriteUnderflow()); }

    // Write
    CharType getw() const  { return *mWriteCursor;   }
    CharType peekw() const { return *mWriteCursor;   }
    CharType takew()       { return *mWriteCursor++; }

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

    // Next
    void nextWriteCursor() { mWriteCursor++; }
    void nextw()           { return nextWriteCursor(); }

    // Get position
    SizeType getWritePosition() const {
        return static_cast<size_t>(mWriteCursor - mBegin);
    }

    SizeType tellw() const    { return getWritePosition(); }
    SizeType getSizeW() const { return getWritePosition(); }
};

}  // namespace JsonFx

// Define default StringOutputStream class type
typedef JsonFx::BasicStringOutputStream<JSONFX_DEFAULT_CHARTYPE>    jfxStringOutputStream;

#endif  /* _JSONFX_STREAM_STRINGOUTPUTSTREAM_H_ */
