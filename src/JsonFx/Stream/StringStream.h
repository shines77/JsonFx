
#ifndef _JSONFX_STREAM_STRINGSTREAM_H_
#define _JSONFX_STREAM_STRINGSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Stream/StringInputStream.h"
#include "JsonFx/Stream/StringOutputStream.h"

namespace JsonFx {

// Forward declaration.
template <typename CharT = JSONFX_DEFAULT_CHARTYPE>
class BasicStringStream;

// Define default BasicStringStream<CharT>.
typedef BasicStringStream<>  StringStream;

template <typename CharT>
#if defined(STRING_STREAM_DERIVED_USE_ROOTCLASS) && (STRING_STREAM_DERIVED_USE_ROOTCLASS != 0)
class BasicStringStream : public BasicStringInputStreamRoot< BasicStringStream<CharT>, CharT >,
                          public BasicStringOutputStreamRoot< BasicStringStream<CharT>, CharT >,
                          public BasicStringStreamRoot<CharT>
#else
class BasicStringStream : public BasicStringInputStream<CharT>,
                          public BasicStringOutputStream<CharT>
#endif
{
#if defined(STRING_STREAM_DERIVED_USE_ROOTCLASS) && (STRING_STREAM_DERIVED_USE_ROOTCLASS != 0)
public:
    typedef typename BasicStringStreamRoot<CharT>::CharType    CharType;
    typedef typename BasicStringStreamRoot<CharT>::SizeType    SizeType;

public:
    BasicStringStream(const CharType * src)
        : BasicStringInputStreamRoot<BasicStringStream<CharT>, CharT>(src),
          BasicStringOutputStreamRoot<BasicStringStream<CharT>, CharT>(src),
          BasicStringStreamRoot<CharT>(src)
    {
        jfx_iostream_trace("00 BasicStringStream<T>::BasicStringStream(const CharType * src);\n");
    }

    BasicStringStream(const void * src)
        : BasicStringInputStreamRoot<BasicStringStream<CharT>, CharT>(src),
          BasicStringOutputStreamRoot<BasicStringStream<CharT>, CharT>(src),
          BasicStringStreamRoot<CharT>(src)
    {
        jfx_iostream_trace("00 BasicStringStream<T>::BasicStringStream(const void * src);\n");
    }
#else
public:
    typedef typename BasicStringInputStream<CharT>::CharType    CharType;
    typedef typename BasicStringInputStream<CharT>::SizeType    SizeType;

public:
    BasicStringStream(const CharType * src)
        : BasicStringInputStream<CharT>(src),
          BasicStringOutputStream<CharT>(src),
          BasicStringStreamRoot<CharT>(src)
    {
        jfx_iostream_trace("00 BasicStringStream<T>::BasicStringStream(const CharType * src);\n");
    }

    BasicStringStream(const void * src)
        : BasicStringInputStream<CharT>(src),
          BasicStringOutputStream<CharT>(src),
          BasicStringStreamRoot<CharT>(src)
    {
        jfx_iostream_trace("00 BasicStringStream<T>::BasicStringStream(const void * src);\n");
    }
#endif  /* defined(STRING_STREAM_DERIVED_USE_ROOTCLASS) */

    ~BasicStringStream() {
        jfx_iostream_trace("01 BasicStringStream<T>::~BasicStringStream();\n");
    }

#if defined(STRING_STREAM_DERIVED_USE_ROOTCLASS) && (STRING_STREAM_DERIVED_USE_ROOTCLASS != 0)
    // BasicStringStreamRoot() Get properties
    CharType * getBegin() const  { return const_cast<CharType *>(mBegin);             }
    void *     getBeginV() const { return reinterpret_cast<void *>(this->getBegin()); }

    // BasicStringStreamRoot() Get properties
    CharType * getEnd() const  {
        BasicStringStreamRoot<CharT> * pThis = static_cast<BasicStringStreamRoot<CharT> *>(const_cast<BasicStringStream<CharT> *>(this));
        return (pThis) ? reinterpret_cast<CharType *>(pThis->getEnd()) : NULL;
    }
    void * getEndV() const {
        BasicStringStreamRoot<CharT> * pThis = static_cast<BasicStringStreamRoot<CharT> *>(const_cast<BasicStringStream<CharT> *>(this));
        return (pThis) ? reinterpret_cast<void *>(pThis->getEndV()) : NULL;
    }
#endif
};

}  // namespace JsonFx

// Define default StringStream class type
typedef JsonFx::BasicStringStream<JSONFX_DEFAULT_CHARTYPE>    jfxStringStream;

#endif  /* _JSONFX_STREAM_STRINGSTREAM_H_ */
