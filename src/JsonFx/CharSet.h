
#ifndef _JSONFX_CHARSET_H_
#define _JSONFX_CHARSET_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "JsonFx/Config.h"

// Define default encoding -- CharSet::UTF8
#define JSONFX_DEFAULT_ENCODING BaseEncoding<JSONFX_DEFAULT_CHARTYPE>

namespace JsonFx {

template <typename _CharType = JSONFX_DEFAULT_CHARTYPE>
class BaseEncoding
{
public:
    typedef _CharType CharType;
};

// Define default Encoding class type
typedef BaseEncoding<>  Encoding;

class CharSet {
public:
    typedef BaseEncoding<char>      UTF8;

#if defined(__linux__)
    typedef BaseEncoding<unsigned short>    UTF16;
    typedef BaseEncoding<unsigned short>    UTF16LE;
    typedef BaseEncoding<unsigned short>    UTF16BE;

    typedef BaseEncoding<wchar_t>   UTF32;
    typedef BaseEncoding<wchar_t>   UTF32LE;
    typedef BaseEncoding<wchar_t>   UTF32BE;
#else
    typedef BaseEncoding<wchar_t>   UTF16;
    typedef BaseEncoding<wchar_t>   UTF16LE;
    typedef BaseEncoding<wchar_t>   UTF16BE;

    typedef BaseEncoding<unsigned>  UTF32;
    typedef BaseEncoding<unsigned>  UTF32LE;
    typedef BaseEncoding<unsigned>  UTF32BE;
#endif
};

// Define default encoding type -- CharSet::UTF8
typedef JSONFX_DEFAULT_ENCODING DefaultEncoding;

}  // namespace JsonFx

#endif  /* !_JSONFX_CHARSET_H_ */
