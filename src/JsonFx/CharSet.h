
#ifndef _JSONFX_CHARSET_H_
#define _JSONFX_CHARSET_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "JsonFx/Config.h"

// Define default encoding -- CharSet::UTF8
#define JSONFX_DEFAULT_ENCODING BasicEncoding<JSONFX_DEFAULT_CHARTYPE>

namespace JsonFx {

template <typename _CharType = JSONFX_DEFAULT_CHARTYPE>
class BasicEncoding
{
public:
    typedef _CharType CharType;
};

// Define default Encoding class type
typedef BasicEncoding<>  Encoding;

class CharSet {
public:
#if 1
    typedef BasicEncoding<char>      UTF8;
#else
    class UTF8 : public BasicEncoding<char> {
    public:
        typedef char CharType;
    };
#endif

#if defined(__linux__)
    typedef BasicEncoding<unsigned short>    UTF16;
    typedef BasicEncoding<unsigned short>    UTF16LE;
    typedef BasicEncoding<unsigned short>    UTF16BE;

    typedef BasicEncoding<wchar_t>   UTF32;
    typedef BasicEncoding<wchar_t>   UTF32LE;
    typedef BasicEncoding<wchar_t>   UTF32BE;
#else
    typedef BasicEncoding<wchar_t>   UTF16;
    typedef BasicEncoding<wchar_t>   UTF16LE;
    typedef BasicEncoding<wchar_t>   UTF16BE;

    typedef BasicEncoding<unsigned>  UTF32;
    typedef BasicEncoding<unsigned>  UTF32LE;
    typedef BasicEncoding<unsigned>  UTF32BE;
#endif
};

#if 1
    typedef BasicEncoding<char>      CharSet_UTF8;
#else
    class CharSet_UTF8 : public BasicEncoding<char> {
    public:
        typedef char CharType;
    };
#endif

#if defined(__linux__)
    typedef BasicEncoding<unsigned short>    CharSet_UTF16;
    typedef BasicEncoding<unsigned short>    CharSet_UTF16LE;
    typedef BasicEncoding<unsigned short>    CharSet_UTF16BE;

    typedef BasicEncoding<wchar_t>   CharSet_UTF32;
    typedef BasicEncoding<wchar_t>   CharSet_UTF32LE;
    typedef BasicEncoding<wchar_t>   CharSet_UTF32BE;
#else
    typedef BasicEncoding<wchar_t>   CharSet_UTF16;
    typedef BasicEncoding<wchar_t>   CharSet_UTF16LE;
    typedef BasicEncoding<wchar_t>   CharSet_UTF16BE;

    typedef BasicEncoding<unsigned>  CharSet_UTF32;
    typedef BasicEncoding<unsigned>  CharSet_UTF32LE;
    typedef BasicEncoding<unsigned>  CharSet_UTF32BE;
#endif

// Define default encoding type -- CharSet::UTF8
typedef JSONFX_DEFAULT_ENCODING DefaultEncoding;

}  // namespace JsonFx

#endif  /* !_JSONFX_CHARSET_H_ */
