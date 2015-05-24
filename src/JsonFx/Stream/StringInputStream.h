
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

// Forward declaration.
template <typename T, typename CharT = JSONFX_DEFAULT_CHARTYPE>
class BasicStringInputStreamRoot;

// Define default BasicStringInputStreamRoot<T, CharT>.
typedef BasicStringInputStreamRoot< BasicStringInputStream<> >  StringInputStreamRoot;

template <typename T, typename CharT>
class BasicStringInputStreamRoot
{
public:
    typedef typename CharT      CharType;
    typedef typename size_t     SizeType;

protected:
    const CharType * mReadCursor;

public:
    BasicStringInputStreamRoot(const CharType * src) : mReadCursor(src) {
        jfx_iostream_trace("00 BasicStringInputStreamRoot<T>::BasicStringInputStreamRoot(const CharType * src);\n");
    }

    BasicStringInputStreamRoot(const void * src) : mReadCursor(reinterpret_cast<const CharType *>(src)) {
        jfx_iostream_trace("00 BasicStringInputStreamRoot<T>::BasicStringInputStreamRoot(const void * src);\n");
    }

    ~BasicStringInputStreamRoot() {
        jfx_iostream_trace("01 BasicStringInputStreamRoot<T>::~BasicStringInputStreamRoot();\n");
    }

public:
    // class T() Get properties
    CharType * getBegin() const  {
        T * pThis = static_cast<T *>(const_cast<BasicStringInputStreamRoot<T, CharT> *>(this));
        return (pThis) ? reinterpret_cast<CharType *>(pThis->getBegin()) : NULL;
    }
    void * getBeginV() const {
        T * pThis = static_cast<T *>(const_cast<BasicStringInputStreamRoot<T, CharT> *>(this));
        return (pThis) ? reinterpret_cast<void *>(pThis->getBeginV()) : NULL;
    }

    CharType * getEnd() const  {
        T * pThis = static_cast<T *>(const_cast<BasicStringInputStreamRoot<T, CharT> *>(this));
        return (pThis) ? reinterpret_cast<CharType *>(pThis->getEnd()) : NULL;
    }
    void * getEndV() const {
        T * pThis = static_cast<T *>(const_cast<BasicStringInputStreamRoot<T, CharT> *>(this));
        return (pThis) ? reinterpret_cast<void *>(pThis->getEndV()) : NULL;
    }

    // Get properties
    CharType * getReadCursor() const  { return const_cast<CharType *>(mReadCursor);       }
    void *     getReadCursorV() const { return reinterpret_cast<void *>(getReadCursor()); }

    CharType * getCurrent() const     { return getReadCursor();  }
    void *     getCurrentV() const    { return getReadCursorV(); }

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
        jimi_assert(mReadCursor < this->getEnd());
        return (*mReadCursor == static_cast<SizeType>('\0'));
    }
    bool isEof() const { return isReadEof(); }

    // Check range
    bool isReadUnderflow() const { return (mReadCursor < mBegin); }
    bool isReadOverflow() const  { return (mReadCursor > this->getEnd()); }
    bool isReadValid() const     { return (isReadOverflow() && isReadUnderflow()); }

    bool underflow() const { return isReadUnderflow(); }
    bool overflow() const  { return isReadOverflow();  }
    bool isValid() const   { return (overflow() && underflow()); }

    // Read
    CharType get() const  { return *mReadCursor;   }
    CharType peek() const { return *mReadCursor;   }
    CharType take()       { return *mReadCursor++; }

    // Next
    void nextReadCursor() { mReadCursor++;           }
    void next()           { return nextReadCursor(); }

    // Get position
    SizeType getReadPosition() const {
        return static_cast<size_t>(mReadCursor - this->getBegin());
    }
    SizeType tell() const    { return getReadPosition(); }
    SizeType getSize() const { return getReadPosition(); }
};

template <typename CharT>
#if defined(STRING_STREAM_DERIVED_USE_ROOTCLASS) && (STRING_STREAM_DERIVED_USE_ROOTCLASS != 0)
class  BasicStringInputStream : public BasicStringInputStreamRoot< BasicStringInputStream<CharT>, CharT >,
                                public BasicStringStreamRoot<CharT>
#else
class BasicStringInputStream : virtual public BasicStringStreamRoot<CharT>
#endif
{
public:
    typedef typename BasicStringStreamRoot<CharT>::CharType    CharType;
    typedef typename BasicStringStreamRoot<CharT>::SizeType    SizeType;

#if !defined(STRING_STREAM_DERIVED_USE_ROOTCLASS) || (STRING_STREAM_DERIVED_USE_ROOTCLASS == 0)
protected:
    const CharType * mReadCursor;
#endif

public:

#if defined(STRING_STREAM_DERIVED_USE_ROOTCLASS) && (STRING_STREAM_DERIVED_USE_ROOTCLASS != 0)
    BasicStringInputStream(const CharType * src)
        : BasicStringInputStreamRoot<BasicStringInputStream<CharT>, CharT>(src),
          BasicStringStreamRoot<CharT>(src)
    {
        jfx_iostream_trace("00 BasicStringInputStream<T>::BasicStringInputStream(const CharType * src);\n");
    }

    BasicStringInputStream(const void * src)
        : BasicStringInputStreamRoot<BasicStringInputStream<CharT>, CharT>(src),
          BasicStringStreamRoot<CharT>(src)
    {
        jfx_iostream_trace("00 BasicStringInputStream<T>::BasicStringInputStream(const void * src);\n");
    }
#else
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
#endif

    ~BasicStringInputStream() {
        jfx_iostream_trace("01 BasicStringInputStream<T>::~BasicStringInputStream();\n");
    }

#if defined(STRING_STREAM_DERIVED_USE_ROOTCLASS) && (STRING_STREAM_DERIVED_USE_ROOTCLASS != 0)

    // BasicStringStreamRoot() Get properties
    CharType * getBegin() const  { return const_cast<CharType *>(mBegin);             }
    void *     getBeginV() const { return reinterpret_cast<void *>(this->getBegin()); }

    // BasicStringStreamRoot() Get properties
    CharType * getEnd() const  {
        BasicStringStreamRoot<CharT> * pThis = static_cast<BasicStringStreamRoot<CharT> *>(const_cast<BasicStringInputStream<CharT> *>(this));
        return (pThis) ? reinterpret_cast<CharType *>(pThis->getEnd()) : NULL;
    }
    void * getEndV() const {
        BasicStringStreamRoot<CharT> * pThis = static_cast<BasicStringStreamRoot<CharT> *>(const_cast<BasicStringInputStream<CharT> *>(this));
        return (pThis) ? reinterpret_cast<void *>(pThis->getEndV()) : NULL;
    }

#else
    // Get properties
    CharType * getReadCursor() const  { return const_cast<CharType *>(mReadCursor);       }
    void *     getReadCursorV() const { return reinterpret_cast<void *>(getReadCursor()); }

    CharType * getCurrent() const     { return getReadCursor();   }
    void *     getCurrentV() const    { return getReadCursorV();  }

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
        jimi_assert(mReadCursor < BasicStringStreamRoot<CharT>::getEnd());
        return (*mReadCursor == static_cast<SizeType>('\0'));
    }
    bool isEof() const { return isReadEof(); }

    // Check range
    bool isReadUnderflow() const { return (mReadCursor < mBegin); }
    bool isReadOverflow() const  { return (mReadCursor > BasicStringStreamRoot<CharT>::getEnd());   }
    bool isReadValid() const     { return (isReadOverflow() && isReadUnderflow()); }

    bool underflow() const { return isReadUnderflow(); }
    bool overflow() const  { return isReadOverflow();  }
    bool isValid() const   { return (overflow() && underflow()); }

    // Read
    CharType get() const  { return *mReadCursor;   }
    CharType peek() const { return *mReadCursor;   }
    CharType take()       { return *mReadCursor++; }

    // Next
    void nextReadCursor() { mReadCursor++;           }
    void next()           { return nextReadCursor(); }

    // Get position
    SizeType getReadPosition() const {
        return static_cast<size_t>(mReadCursor - mBegin);
    }
    SizeType tell() const    { return getReadPosition(); }
    SizeType getSize() const { return getReadPosition(); }

#endif  /* defined(STRING_STREAM_DERIVED_USE_ROOTCLASS) */
};

}  // namespace JsonFx

// Define default StringInputStream class type
typedef JsonFx::BasicStringInputStream<JSONFX_DEFAULT_CHARTYPE>    jfxStringInputStream;

#endif  /* _JSONFX_STREAM_STRINGINPUTSTREAM_H_ */
