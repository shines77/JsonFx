
#ifndef _JSONFX_STREAM_STRINGSTREAMROOT_H_
#define _JSONFX_STREAM_STRINGSTREAMROOT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Stream/StreamRoot.h"

#define STRING_STREAM_DERIVED_USE_ROOTCLASS     1

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
    static const size_t kSize             = static_cast<size_t>(-1);

public:
    BasicStringStreamRoot(const CharType * src) : BasicStreamRoot<CharT>(src)
    {
        jfx_iostream_trace("00 BasicStringStreamRoot<T>::BasicStringStreamRoot(const CharType * src);\n");
    }

    BasicStringStreamRoot(const void * src) : BasicStreamRoot<CharT>(src)
    {
        jfx_iostream_trace("00 BasicStringStreamRoot<T>::BasicStringStreamRoot(const void * src);\n");
    }

    ~BasicStringStreamRoot() {
        jfx_iostream_trace("01 BasicStringStreamRoot<T>::~BasicStringStreamRoot();\n");
    }

    // Get properties
    CharType *       getEnd() const   { return reinterpret_cast<CharType *>(kMaxMemoryAddress);       }
    const CharType * getEndC() const  { return reinterpret_cast<const CharType *>(kMaxMemoryAddress); }
    void *           getEndV() const  { return reinterpret_cast<void *>(kMaxMemoryAddress);           }
    const void *     getEndCV() const { return reinterpret_cast<const void *>(kMaxMemoryAddress);     }

    //bool isValid() const  { return (mBegin != NULL); }
    bool isNormal() const { return true; }

    //SizeType getSize() const { return kSize; }
};

}  // namespace JsonFx

// Define default StringStreamRoot class type
typedef JsonFx::BasicStringStreamRoot<JSONFX_DEFAULT_CHARTYPE>    jfxStringStreamRoot;

#endif  /* _JSONFX_STREAM_STRINGSTREAMROOT_H_ */
