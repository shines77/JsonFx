
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
template <typename T, typename CharT = JSONFX_DEFAULT_CHARTYPE>
class BasicStringOutputStreamRoot;

// Define default BasicStringOutputStreamRoot<T, CharT>.
typedef BasicStringOutputStreamRoot< BasicStringOutputStream<> >   StringOutputStreamRoot;

template <typename T, typename CharT>
class BasicStringOutputStreamRoot
{
public:
    typedef typename CharT      CharType;
    typedef typename size_t     SizeType;

protected:
    CharType * mWriteCursor;

public:
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

public:
    // class T() Get properties
    CharType * getBegin() const  {
        T * pThis = static_cast<T *>(const_cast<BasicStringOutputStreamRoot<T, CharT> *>(this));
        return (pThis) ? reinterpret_cast<CharType *>(pThis->getBegin()) : NULL;
    }
    void * getBeginV() const {
        T * pThis = static_cast<T *>(const_cast<BasicStringOutputStreamRoot<T, CharT> *>(this));
        return (pThis) ? reinterpret_cast<void *>(pThis->getBeginV()) : NULL;
    }

    CharType * getEnd() const  {
        T * pThis = static_cast<T *>(const_cast<BasicStringOutputStreamRoot<T, CharT> *>(this));
        return (pThis) ? reinterpret_cast<CharType *>(pThis->getEnd()) : NULL;
    }
    void * getEndV() const {
        T * pThis = static_cast<T *>(const_cast<BasicStringOutputStreamRoot<T, CharT> *>(this));
        return (pThis) ? reinterpret_cast<void *>(pThis->getEndV()) : NULL;
    }

    // Get properties
    CharType * getWriteCursor() const  { return mWriteCursor; }
    void *     getWriteCursorV() const { return reinterpret_cast<void *>(mWriteCursor); }

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
        jimi_assert(mWriteCursor < this->getEnd());
        return (*mWriteCursor == static_cast<SizeType>('\0'));
    }
    bool isEofW() const { return isWriteEof(); }

    // Check range
    bool isWriteUnderflow() const { return (mWriteCursor < mBegin);   }
    bool isWriteOverflow() const  { return (mWriteCursor > this->getEnd()); }
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
        return static_cast<size_t>(mWriteCursor - this->getBegin());
    }

    SizeType tellw() const    { return getWritePosition(); }
    SizeType getSizeW() const { return getWritePosition(); }
};

template <typename CharT>
#if defined(STRING_STREAM_DERIVED_USE_ROOTCLASS) && (STRING_STREAM_DERIVED_USE_ROOTCLASS != 0)
class BasicStringOutputStream : public BasicStringOutputStreamRoot< BasicStringOutputStream<CharT>, CharT >,
                                public BasicStringStreamRoot<CharT>
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
        : BasicStringOutputStreamRoot<BasicStringOutputStream<CharT>, CharT>(src),
          BasicStringStreamRoot<CharT>(src)
    {
        jfx_iostream_trace("00 BasicStringOutputStream<T>::BasicStringOutputStream(const CharType * src);\n");
    }

    BasicStringOutputStream(const void * src)
        : BasicStringOutputStreamRoot<BasicStringOutputStream<CharT>, CharT>(src),
          BasicStringStreamRoot<CharT>(src),
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

#if defined(STRING_STREAM_DERIVED_USE_ROOTCLASS) && (STRING_STREAM_DERIVED_USE_ROOTCLASS != 0)

    // BasicStringStreamRoot() Get properties
    CharType * getBegin() const  { return const_cast<CharType *>mBegin;               }
    void *     getBeginV() const { return reinterpret_cast<void *>(this->getBegin()); }

    // BasicStringStreamRoot() Get properties
    CharType * getEnd() const  {
        BasicStringStreamRoot<CharT> * pThis = static_cast<BasicStringStreamRoot<CharT> *>(const_cast<BasicStringOutputStream<CharT> *>(this));
        return (pThis) ? reinterpret_cast<CharType *>(pThis->getEnd()) : NULL;
    }
    void * getEndV() const {
        BasicStringStreamRoot<CharT> * pThis = static_cast<BasicStringStreamRoot<CharT> *>(const_cast<BasicStringOutputStream<CharT> *>(this));
        return (pThis) ? reinterpret_cast<void *>(pThis->getEndV()) : NULL;
    }

#else
    // Get properties
    CharType * getWriteCursor() const  { return mWriteCursor; }
    void *     getWriteCursorV() const { return reinterpret_cast<void *>(mWriteCursor); }

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
        jimi_assert(mWriteCursor < BasicStringStreamRoot<CharT>::getEnd());
        return (*mWriteCursor == static_cast<SizeType>('\0'));
    }
    bool isEofW() const { return isWriteEof(); }

    // Check range
    bool isWriteUnderflow() const { return (mWriteCursor < mBegin);   }
    bool isWriteOverflow() const  { return (mWriteCursor > BasicStringStreamRoot<CharT>::getEnd()); }
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

#endif  /* defined(STRING_STREAM_DERIVED_USE_ROOTCLASS) */
};

}  // namespace JsonFx

// Define default StringOutputStream class type
typedef JsonFx::BasicStringOutputStream<JSONFX_DEFAULT_CHARTYPE>    jfxStringOutputStream;

#endif  /* _JSONFX_STREAM_STRINGOUTPUTSTREAM_H_ */
