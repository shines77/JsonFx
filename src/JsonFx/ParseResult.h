
#ifndef _JSONFX_PRASERESULT_H_
#define _JSONFX_PRASERESULT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "jimi/basic/stdsize.h"

namespace JsonFx {

enum ParseErrorCode {
    kNoneParseError = 0,
    kAnyParseError,
    kKeyStringMissQuoteError,
    kValueStringMissQuoteError,
    kLastParseError
};

// Forward declaration.
template <typename EncodingT>
class BasicParseResult;

// Define default Writer class type
typedef BasicParseResult<DefaultEncoding>   ParseResult;

template <typename EncodingT>
class BasicParseResult {
private:
    ParseErrorCode  mErrCode;
    size_t          mErrLine;
    size_t          mErrOffset;
    size_t          mReserve;

public:
    BasicParseResult() : mErrCode(kNoneParseError), mErrLine(0), mErrOffset(0) {}
    BasicParseResult(const BasicParseResult & src)
        : mErrCode(src.mErrCode), mErrLine(src.mErrLine), mErrOffset(src.mErrOffset) {}
    BasicParseResult(ParseErrorCode code, ssize_t line, ssize_t offset)
        : mErrCode(code), mErrLine(line), mErrOffset(offset) {}
    ~BasicParseResult() {}

    BasicParseResult & operator = (const BasicParseResult & rhs) {
        this->mErrCode   = rhs.mErrCode;
        this->mErrLine   = rhs.mErrLine;
        this->mErrOffset = rhs.mErrOffset;
        return (*this);
    }

    bool hasError() const { return (getError() != kNoneParseError); }
    operator bool() const { return !hasError(); }

    ParseErrorCode getError() const { return mErrCode; }

    bool operator == (const BasicParseResult & that) const {
        return mErrCode == that.mErrCode;
    }
    bool operator == (ParseErrorCode code) const {
        return mErrCode == code;
    }
    friend bool operator == (ParseErrorCode code, const BasicParseResult & err) {
        return code == err.mErrCode;
    }

    void setError(ParseErrorCode code, size_t line = 0, size_t offset = 0) {
        mErrCode = code; mErrLine = line; mErrOffset = offset;
    }

    void clear() {
        setError(kNoneParseError);
    }
};

}  // namespace JsonFx

// Define default ParseResult class type
typedef JsonFx::BasicParseResult<JsonFx::DefaultEncoding>   jfxParseResult;

#endif  /* !_JSONFX_PRASERESULT_H_ */
