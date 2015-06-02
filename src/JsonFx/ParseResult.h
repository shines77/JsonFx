
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
    size_t          mErrOffset;

public:
    BasicParseResult() : mErrCode(kNoneParseError), mErrOffset(0) {}
    BasicParseResult(const BasicParseResult & src)
        : mErrCode(src.mErrCode), mErrOffset(src.mErrOffset) {}
    BasicParseResult(ParseErrorCode code, size_t offset)
        : mErrCode(code), mErrOffset(offset) {}
    ~BasicParseResult() {}

    BasicParseResult & operator = (const BasicParseResult & rhs) {
        this->mErrCode   = rhs.mErrCode;
        this->mErrOffset = rhs.mErrOffset;
        return (*this);
    }

    bool hasError() const { return (getError() != kNoneParseError); }
    operator bool() const { return !hasError(); }

    ParseErrorCode getError() const       { return mErrCode;   }
    size_t         getErrorOffset() const { return mErrOffset; }

    bool operator == (const BasicParseResult & that) const {
        return mErrCode == that.mErrCode;
    }
    bool operator == (ParseErrorCode code) const {
        return mErrCode == code;
    }
    friend bool operator == (ParseErrorCode code, const BasicParseResult & err) {
        return code == err.mErrCode;
    }

    void setError(ParseErrorCode code) {
        mErrCode = code;
    }
    void setError(size_t offset) {
        mErrOffset = offset;
    }
    void setError(ParseErrorCode code, size_t offset) {
        mErrCode = code; mErrOffset = offset;
    }

    void setErrorOffset(size_t offset) {
        mErrOffset = offset;
    }

    void clear() {
        setError(kNoneParseError, 0);
    }
};

template <typename EncodingT>
class BasicParseResultEx {
private:
    ParseErrorCode  mErrCode;
    size_t          mErrOffset;
    size_t          mErrLine;
    size_t          mReserve;

public:
    BasicParseResultEx() : mErrCode(kNoneParseError), mErrOffset(0), mErrLine(0) {}
    BasicParseResultEx(const BasicParseResult & src)
        : mErrCode(src.mErrCode), mErrOffset(src.mErrOffset), mErrLine(src.mErrLine) {}
    BasicParseResultEx(ParseErrorCode code, ssize_t line, ssize_t offset)
        : mErrCode(code), mErrOffset(offset), mErrLine(line) {}
    ~BasicParseResultEx() {}

    BasicParseResultEx & operator = (const BasicParseResultEx & rhs) {
        this->mErrCode   = rhs.mErrCode;
        this->mErrOffset = rhs.mErrOffset;
        this->mErrLine   = rhs.mErrLine;
        return (*this);
    }

    bool hasError() const { return (getError() != kNoneParseError); }
    operator bool() const { return !hasError(); }

    ParseErrorCode getError() const       { return mErrCode;   }
    size_t         getErrorLine() const   { return mErrLine;   }
    size_t         getErrorOffset() const { return mErrOffset; }

    bool operator == (const BasicParseResultEx & that) const {
        return mErrCode == that.mErrCode;
    }
    bool operator == (ParseErrorCode code) const {
        return mErrCode == code;
    }
    friend bool operator == (ParseErrorCode code, const BasicParseResult & err) {
        return code == err.mErrCode;
    }

    void setError(ParseErrorCode code) {
        mErrCode = code;
    }
    void setError(size_t line, size_t offset) {
        mErrOffset = offset; mErrLine = line;
    }
    void setError(ParseErrorCode code, size_t line, size_t offset) {
        mErrCode = code; mErrOffset = offset; mErrLine = line;
    }

    void setErrorLine(size_t line) {
        mErrLine = line;
    }
    void setErrorOffset(size_t offset) {
        mErrOffset = offset;
    }

    void clear() {
        setError(kNoneParseError, 0, 0);
    }

    void incLine() {
        mErrLine++;
    }

    void addLines(int lines) {
        mErrLine += lines;
    }
};

}  // namespace JsonFx

// Define default ParseResult class type
typedef JsonFx::BasicParseResult<JsonFx::DefaultEncoding>   jfxParseResult;

#endif  /* !_JSONFX_PRASERESULT_H_ */
