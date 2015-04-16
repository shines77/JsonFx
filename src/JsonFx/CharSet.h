
#ifndef _JSONFX_CHARSET_H_
#define _JSONFX_CHARSET_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "JsonFx/Config.h"

/* Whether use class wrapper in encoding classes? */
#define ENCODING_USE_CLASS_WRAPPER      1

namespace JsonFx {

enum {
    kUTFUnknown,
    kUTF8,

    kUTF16,
    kUTF16LE,
    kUTF16BE,

    kUTF32,
    kUTF32LE,
    kUTF32BE,

    kUTFMax
};

template <typename _CharType = JSONFX_DEFAULT_CHARTYPE,
          size_t _EncodingType = kUTFUnknown>
struct BasicEncoding
{
public:
    typedef _CharType CharType;
    enum { type = _EncodingType };
};

// Define default encoding -- BasicEncoding<char, kUTF8>
#define JSONFX_DEFAULT_ENCODING     BasicEncoding<JSONFX_DEFAULT_CHARTYPE, kUTF8>

// Define default Encoding class type
typedef BasicEncoding<JSONFX_DEFAULT_CHARTYPE, kUTF8>  Encoding;

// Define default encoding type -- CharSet::UTF8
#if defined(JSONFX_DEFAULT_ENCODING)
typedef JSONFX_DEFAULT_ENCODING     DefaultEncoding;
#endif

#if defined(ENCODING_USE_CLASS_WRAPPER) && (ENCODING_USE_CLASS_WRAPPER != 0)

#if defined(_MSC_VER) && (_MSC_VER < 1600)
#define DECLEAR_ENCODING_CLASS(encodingName, charType, encodingType)        \
    struct encodingName : public BasicEncoding<charType, encodingType> {    \
    public:                                                                 \
        typedef BasicEncoding<charType>::CharType CharType;                 \
        enum { type = encodingType };                                       \
    }
#else
#define DECLEAR_ENCODING_CLASS(encodingName, charType, encodingType)        \
    struct encodingName : public BasicEncoding<charType, encodingType> {    \
    public:                                                                 \
        typedef typename BasicEncoding<charType>::CharType CharType;        \
        enum { type = encodingType };                                       \
    }
#endif  /* _MSC_VER < 1600 */

struct CharSet {
public:
    // utf-8
    DECLEAR_ENCODING_CLASS(UTF8, char, kUTF8);

#if defined(__linux__)
    // utf-16
    DECLEAR_ENCODING_CLASS(UTF16,   unsigned short, kUTF16);
    DECLEAR_ENCODING_CLASS(UTF16LE, unsigned short, kUTF16LE);
    DECLEAR_ENCODING_CLASS(UTF16BE, unsigned short, kUTF16BE);

    // utf-32
    DECLEAR_ENCODING_CLASS(UTF32,   wchar_t,  kUTF32);
    DECLEAR_ENCODING_CLASS(UTF32LE, wchar_t,  kUTF32LE);
    DECLEAR_ENCODING_CLASS(UTF32BE, wchar_t,  kUTF32BE);
#else  /* !__linux__ */
    // utf-16
    DECLEAR_ENCODING_CLASS(UTF16,   wchar_t,  kUTF16);
    DECLEAR_ENCODING_CLASS(UTF16LE, wchar_t,  kUTF16LE);
    DECLEAR_ENCODING_CLASS(UTF16BE, wchar_t,  kUTF16BE);

    // utf-32
    DECLEAR_ENCODING_CLASS(UTF32,   unsigned, kUTF32);
    DECLEAR_ENCODING_CLASS(UTF32LE, unsigned, kUTF32LE);
    DECLEAR_ENCODING_CLASS(UTF32BE, unsigned, kUTF32BE);
#endif  /* __linux__ */
};

#undef DECLEAR_ENCODING_CLASS

typedef CharSet::UTF8       _UTF8;

typedef CharSet::UTF16      _UTF16;
typedef CharSet::UTF16LE    _UTF16LE;
typedef CharSet::UTF16BE    _UTF16BE;

typedef CharSet::UTF32      _UTF32;
typedef CharSet::UTF32LE    _UTF32LE;
typedef CharSet::UTF32BE    _UTF32BE;

#else   /* !defined(ENCODING_USE_CLASS_WRAPPER) */

struct CharSet {
public:
    typedef BasicEncoding<char, kUTF8>  UTF8;

#if defined(__linux__)
    typedef BasicEncoding<unsigned short, kUTF16>   UTF16;
    typedef BasicEncoding<unsigned short, kUTF16LE> UTF16LE;
    typedef BasicEncoding<unsigned short, kUTF16BE> UTF16BE;

    typedef BasicEncoding<wchar_t, kUTF32>      UTF32;
    typedef BasicEncoding<wchar_t, kUTF32LE>    UTF32LE;
    typedef BasicEncoding<wchar_t, kUTF32BE>    UTF32BE;
#else  /* !__linux__ */
    typedef BasicEncoding<wchar_t, kUTF16>      UTF16;
    typedef BasicEncoding<wchar_t, kUTF16LE>    UTF16LE;
    typedef BasicEncoding<wchar_t, kUTF16BE>    UTF16BE;

    typedef BasicEncoding<unsigned, kUTF32>     UTF32;
    typedef BasicEncoding<unsigned, kUTF32LE>   UTF32LE;
    typedef BasicEncoding<unsigned, kUTF32BE>   UTF32BE;
#endif  /* __linux__ */
};

    typedef BasicEncoding<char, kUTF8>  _UTF8;

#if defined(__linux__)
    typedef BasicEncoding<unsigned short, kUTF16>   _UTF16;
    typedef BasicEncoding<unsigned short, kUTF16LE> _UTF16LE;
    typedef BasicEncoding<unsigned short, kUTF16BE> _UTF16BE;

    typedef BasicEncoding<wchar_t, kUTF32>      _UTF32;
    typedef BasicEncoding<wchar_t, kUTF32LE>    _UTF32LE;
    typedef BasicEncoding<wchar_t, kUTF32BE>    _UTF32BE;
#else  /* !__linux__ */
    typedef BasicEncoding<wchar_t, kUTF16>      _UTF16;
    typedef BasicEncoding<wchar_t, kUTF16LE>    _UTF16LE;
    typedef BasicEncoding<wchar_t, kUTF16BE>    _UTF16BE;

    typedef BasicEncoding<unsigned, kUTF32>     _UTF32;
    typedef BasicEncoding<unsigned, kUTF32LE>   _UTF32LE;
    typedef BasicEncoding<unsigned, kUTF32BE>   _UTF32BE;
#endif  /* __linux__ */

#endif  /* defined(ENCODING_USE_CLASS_WRAPPER) */

}  // namespace JsonFx

#endif  /* !_JSONFX_CHARSET_H_ */
