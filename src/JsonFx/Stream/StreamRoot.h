
#ifndef _JSONFX_STREAM_STREAMROOT_H_
#define _JSONFX_STREAM_STREAMROOT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "jimi/basic/stdsize.h"

#include "JsonFx/Config.h"

#ifndef STREAM_GET_CHARSIZE
#define STREAM_GET_CHARSIZE(size)   ((size) / sizeof(CharType))
#endif

#ifndef STREAM_ROUND_CHARSIZE
#define STREAM_ROUND_CHARSIZE(size) ((size) / sizeof(CharType) * sizeof(CharType))
#endif

namespace JsonFx {

// Forward declaration.
template <typename CharT = JSONFX_DEFAULT_CHARTYPE>
class BasicStreamRoot;

// Define default BasicStreamRoot<CharT>.
typedef BasicStreamRoot<>  StreamRoot;

template <typename CharT>
class BasicStreamRoot
{
public:
    typedef CharT   CharType;
    typedef size_t  SizeType;

protected:
    const CharType * mBegin;

public:
    BasicStreamRoot(const CharType * src) : mBegin(src)
    {
        jfx_iostream_trace("00 BasicStreamRoot<T>::BasicStreamRoot(const CharType * src);\n");
    }

    BasicStreamRoot(const void * src) : mBegin(reinterpret_cast<const CharType *>(src))
    {
        jfx_iostream_trace("00 BasicStreamRoot<T>::BasicStreamRoot(const void * src);\n");
    }

    ~BasicStreamRoot() {
        jfx_iostream_trace("01 BasicStreamRoot<T>::~BasicStreamRoot();\n");
    }

    // Get properties
    CharType *       getBegin() const   { return const_cast<CharType *>mBegin; }
    const CharType * getBeginC() const  { return mBegin; }
    void *           getBeginV() const  { return reinterpret_cast<void *>(getBegin()); }
    const void *     getBeginCV() const { return const_cast<const void *>(getBegin()); }

    // Status
    bool isValid() const  { return (mBegin != NULL); }
};

}  // namespace JsonFx

// Define default StringStream class type
typedef JsonFx::BasicStreamRoot<JSONFX_DEFAULT_CHARTYPE>    jfxStreamRoot;

#endif  /* _JSONFX_STREAM_STREAMROOT_H_ */
