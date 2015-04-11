
#ifndef _JSONFX_CHARSET_H_
#define _JSONFX_CHARSET_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "JsonFx/Config.h"

// Define default encoding -- CharSet::UTF8
#define JSONFX_DEFAULT_ENCODING BasicEncoding<JSONFX_DEFAULT_CHARTYPE>

namespace JsonFx {

enum {
    kUTFUndefine,
    kUTF8,

    kUTF16,
    kUTF16LE,
    kUTF16BE,

    kUTF32,
    kUTF32LE,
    kUTF32BE,

    kUTFMax
};

template <typename _CharType = JSONFX_DEFAULT_CHARTYPE>
struct BasicEncoding
{
public:
    typedef _CharType CharType;
    enum { type = kUTFUndefine };
};

// Define default Encoding class type
typedef BasicEncoding<>  Encoding;

struct CharSet {
public:

#if 0
    typedef BasicEncoding<char>      UTF8;

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

#else

#if defined(_MSC_VER) && (_MSC_VER < 1600)
#define DECLEAR_ENCODING_CLASS(encodingName, charType, encodingType) \
    struct encodingName : public BasicEncoding<charType> {           \
    public:                                                          \
        typedef BasicEncoding<charType>::CharType CharType;          \
        enum { type = encodingType };                                \
    }
#else
    struct encodingName : public BasicEncoding<charType> {           \
    public:                                                          \
        typedef typename BasicEncoding<charType>::CharType CharType; \
        enum { type = encodingType };                                \
    }
#endif

    // utf-8
    DECLEAR_ENCODING_CLASS(UTF8, char, kUTF8);

#if defined(__linux__)
    // utf-16
    DECLEAR_ENCODING_CLASS(UTF16,   unsigned short, kUTF16);
    DECLEAR_ENCODING_CLASS(UTF16LE, unsigned short, kUTF16LE);
    DECLEAR_ENCODING_CLASS(UTF16BE, unsigned short, kUTF16BE);

    // utf-32
    DECLEAR_ENCODING_CLASS(UTF32,   wchar_t, kUTF32);
    DECLEAR_ENCODING_CLASS(UTF32LE, wchar_t, kUTF32LE);
    DECLEAR_ENCODING_CLASS(UTF32BE, wchar_t, kUTF32BE);
#else
    // utf-16
    DECLEAR_ENCODING_CLASS(UTF16,   wchar_t, kUTF16);
    DECLEAR_ENCODING_CLASS(UTF16LE, wchar_t, kUTF16LE);
    DECLEAR_ENCODING_CLASS(UTF16BE, wchar_t, kUTF16BE);

    // utf-32
    DECLEAR_ENCODING_CLASS(UTF32,   unsigned, kUTF32);
    DECLEAR_ENCODING_CLASS(UTF32LE, unsigned, kUTF32LE);
    DECLEAR_ENCODING_CLASS(UTF32BE, unsigned, kUTF32BE);
#endif

#undef DECLEAR_ENCODING_CLASS

#endif
};

#if 1
    typedef BasicEncoding<char>      CharSet_UTF8;
#else
    struct CharSet_UTF8 : public BasicEncoding<char> {
    public:
        typedef typename BasicEncoding<char>::CharType CharType;
        enum { type = kUTF8 };
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
