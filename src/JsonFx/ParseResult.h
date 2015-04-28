
#ifndef _JSONFX_PRASERESULT_H_
#define _JSONFX_PRASERESULT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

namespace JsonFx {

enum {
    kParseError_None,
    kParseError_Max
};

// Forward declaration.
template <typename EncodingT>
class BasicParseResult;

// Define default Writer class type
typedef BasicParseResult<DefaultEncoding>   ParseResult;

template <typename EncodingT>
class BasicParseResult {
public:
    BasicParseResult() : mErrno(kParseError_None), mErrLine(-1), nErrOffset(-1) {}
    ~BasicParseResult() {}

private:
    int     mErrno;
    int     mErrLine;
    int     nErrOffset;
};

}  // namespace JsonFx

// Define default ParseResult class type
typedef JsonFx::BasicParseResult<JsonFx::DefaultEncoding>   jfxParseResult;

#endif  /* !_JSONFX_PRASERESULT_H_ */
