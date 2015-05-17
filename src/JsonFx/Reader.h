
#ifndef _JSONFX_READER_H_
#define _JSONFX_READER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Allocator.h"
#include "JsonFx/Stack.h"
#include "JsonFx/ParseResult.h"
#include "JsonFx/Internal/Traits.h"

#define JSONFX_DEFAULT_PARSE_FLAGS      kNoneParseFlag

namespace JsonFx {

enum ParseFlags {
    kNoneParseFlag              = 0,
    kInsituParseFlag            = 1 << 0,
    kNoStringEscapeParseFlags   = 1 << 8,
    kAllowSingleQuotesParseFlag = 1 << 9,
    kMaxParseFlags              = 0x80000000U,
    kDefaultParseFlags          = JSONFX_DEFAULT_PARSE_FLAGS
};

// Forward declaration.
template <uint64_t parseFlags,
          typename SourceEncodingT,
          typename TargetEncodingT,
          typename PoolAllocatorT = DefaultPoolAllocator,
          typename StackAllocatorT = DefaultAllocator>
class BasicReader;

// Define default Reader class type
typedef BasicReader<kDefaultParseFlags, DefaultEncoding, DefaultEncoding>   Reader;

// BasicReaderHandler
template<typename EncodingT = DefaultEncoding, typename Derived = void>
struct BasicReaderHandler {
    typedef size_t                          SizeType;
    typedef typename EncodingT::CharType    CharType;
    typedef typename internal::SelectIf<internal::IsSame<Derived, void>, BasicReaderHandler, Derived>::Type Override;

    bool saxDefault()           { return true; }
    bool saxNull()              { return static_cast<Override &>(*this).saxDefault(); }
    bool saxBool(bool)          { return static_cast<Override &>(*this).saxDefault(); }
    bool saxInt(int)            { return static_cast<Override &>(*this).saxDefault(); }
    bool saxUint(unsigned)      { return static_cast<Override &>(*this).saxDefault(); }
    bool saxInt64(int64_t)      { return static_cast<Override &>(*this).saxDefault(); }
    bool saxUint64(uint64_t)    { return static_cast<Override &>(*this).saxDefault(); }
    bool saxDouble(double)      { return static_cast<Override &>(*this).saxDefault(); }
    bool saxString(const CharType *, SizeType, bool)
                                { return static_cast<Override &>(*this).saxDefault(); }
    bool saxStartObject()       { return static_cast<Override &>(*this).saxDefault(); }
    bool saxKey(const CharType * str, SizeType len, bool copy)
                                { return static_cast<Override &>(*this).saxString(str, len, copy); }
    bool saxEndObject(SizeType) { return static_cast<Override &>(*this).saxDefault(); }
    bool saxStartArray()        { return static_cast<Override &>(*this).saxDefault(); }
    bool saxEndArray(SizeType)  { return static_cast<Override &>(*this).saxDefault(); }
};

template <uint64_t parseFlags,
          typename SourceEncodingT,
          typename TargetEncodingT,
          typename PoolAllocatorT /* = DefaultPoolAllocator */,
          typename StackAllocatorT /* = DefaultAllocator */>
class BasicReader {
public:
    typedef typename SourceEncodingT::CharType  CharType;   //!< SourceEncoding character type
    typedef PoolAllocatorT                      PoolAllocatorType;  //!< Pool allocator type from template parameter.
    typedef StackAllocatorT                     StackAllocatorType; //!< Stack allocator type from template parameter.
    typedef BasicStack<PoolAllocatorT>          StackType;
    typedef BasicParseResult<SourceEncodingT>   ParseResultType;

    static const size_t kDefaultStackCapacity = 256;

private:
    PoolAllocatorType * mPoolAllocator;
    bool                mPoolAllocatorNeedFree;
    StackType           mStack;
    ParseResultType     mParseResult;

public:
    BasicReader(const PoolAllocatorType * poolAllocator = NULL,
                bool poolAllocatorNeedFree = false,
                const PoolAllocatorType * stackAllocator = NULL,
                size_t stackCapacity = kDefaultStackCapacity)
        : mPoolAllocator(const_cast<PoolAllocatorType *>(poolAllocator)),
          mPoolAllocatorNeedFree(poolAllocatorNeedFree),
          mStack(poolAllocator, stackCapacity), mParseResult()
    {
        /* Do nothing! */
        initPoolAllocator(poolAllocator, poolAllocatorNeedFree);
    }

    ~BasicReader() {
        destroy();
    }

private:
    void destroy() {
        if (this->mPoolAllocatorNeedFree) {
            if (this->mPoolAllocator) {
                delete this->mPoolAllocator;
                this->mPoolAllocator = NULL;
            }
        }
    }

    void initPoolAllocator(const PoolAllocatorType * poolAllocator, bool poolAllocatorNeedFree) {
        if (poolAllocator == NULL) {
            PoolAllocatorType * newPoolAllocator = new PoolAllocatorType();
            jimi_assert(newPoolAllocator != NULL);
            if (newPoolAllocator != NULL) {
                mPoolAllocator = newPoolAllocator;
                mPoolAllocatorNeedFree = poolAllocatorNeedFree;
            }
        }
    }

public:
    bool hasError() const { return mParseResult.hasError(); }
    ParseErrorCode getError() const {
        return mParseResult.getError();
    }
    void setError(ParseErrorCode code, size_t line = 0, size_t offset = 0) {
        return mParseResult.setError(code, line, offset);
    }

    // The whitespace chars including " \t\n\r"
    template <typename InuptStreamT>
    JIMI_FORCEINLINE
    bool isWhiteSpaces(const InuptStreamT & is) const {
        // '\t' = 0x07, '\n' = 0x0A, '\r' = 0x0D
        return ((is.peek() == _Ch(' ')) || (is.peek() >= _Ch('\t') && is.peek() <= _Ch('\r')));
    }

    template <typename InuptStreamT>
    JIMI_FORCEINLINE
    void skipWhiteSpaces(InuptStreamT & is) {
        // '\t' = 0x07, '\n' = 0x0A, '\r' = 0x0D
#if 1
        while ((is.peek() == _Ch(' ')) || (is.peek() >= _Ch('\t') && is.peek() <= _Ch('\r'))) {
            is.next();
        }
#else
        while (isWhiteSpaces(is)) {
            is.next();
        }
#endif
    }

    template <typename InputStreamT, typename ReaderHandlerT>
    void parseObject(InputStreamT & is, ReaderHandlerT & handler) {
        //
    }

    template <typename InputStreamT, typename ReaderHandlerT>
    void parseArray(InputStreamT & is, ReaderHandlerT & handler) {
        //
    }

    template <CharType beginToken, typename InputStreamT, typename ReaderHandlerT>
    void parseInsituString(InputStreamT & is, ReaderHandlerT & handler, bool isKey = true) {
        //
    }

    template <CharType beginToken, typename InputStreamT, typename ReaderHandlerT>
    void parseString(InputStreamT & is, ReaderHandlerT & handler, bool isKey = true) {
        jimi_assert(mPoolAllocator != NULL);
        // The size of string length field
        static const size_t kSizeOfHeadField = sizeof(uint32_t) + sizeof(uint32_t);
        // Reserve string size
        static const size_t kReserveStringSize = 8;
        CharType * cursor = reinterpret_cast<CharType *>(mPoolAllocator->skip(kSizeOfHeadField, kReserveStringSize));
        jimi_assert(cursor != NULL);
        CharType * begin  = cursor;
        CharType * bottom = reinterpret_cast<CharType *>(mPoolAllocator->getChunkBottom());

        while (is.peek() != beginToken && is.peek() != _Ch('\0')) {
            if (cursor < bottom) {
                *cursor++ = is.take();
            }
            else {
                // The remain space in the active chunk is not enough to store the string's
                // characters, so we allocate a new chunk to store it.
                CharType * newCursor = reinterpret_cast<CharType *>(mPoolAllocator->addNewChunkAndSkip(kSizeOfHeadField, kReserveStringSize));
                CharType * newBegin  = newCursor;
                jimi_assert(newCursor != NULL);
                while (begin != cursor) {
                    *newCursor++ = *begin++;
                }
                cursor  = newCursor;
                begin   = newBegin;
                bottom  = reinterpret_cast<CharType *>(mPoolAllocator->getChunkBottom());

                while (is.peek() != beginToken && is.peek() != _Ch('\0')) {
                    if (cursor < bottom) {
                        *cursor++ = is.take();
                    }
                    else {
                        // If it need allocate memory second time, mean the string's length
                        // is more than PoolAllocator's kChunkCapacoty bytes, so we find
                        // out the string's length first, and allocate the enough memory
                        // to fill the string's characters.
                        size_t lenScanned = cursor - begin;
                        this->parseLargeString<beginToken>(is, handler, isKey, lenScanned);
                    }
                }
            }
        }
        // It's the ending of string token.
        if (is.peek() == beginToken) {
            is.next();
            *cursor = _Ch('\0');
            ++cursor;
            jimi_assert(cursor >= begin);
            size_t length = cursor - begin;
            uint32_t * pHeadInfo = reinterpret_cast<uint32_t *>(reinterpret_cast<char *>(begin) - kSizeOfHeadField);
            jimi_assert(pHeadInfo != NULL);
            *pHeadInfo = static_cast<uint32_t>(kConstStringFlags);
            pHeadInfo++;
            *pHeadInfo = static_cast<uint32_t>(length);
            mPoolAllocator->allocate(kSizeOfHeadField + length * sizeof(CharType));
        }
        else {
            // Error: The tail token is not match.
            if (isKey)
                this->setError(kKeyStringMissQuoteError);
            else
                this->setError(kValueStringMissQuoteError);
        }
    }

    template <CharType beginToken, typename InputStreamT, typename ReaderHandlerT>
    JIMI_NOINLINE_DECLARE(void) parseLargeString(InputStreamT & is, ReaderHandlerT & handler,
                                                 bool isKey, size_t lenScanned) {
        // The size of string length field
        static const size_t kSizeOfHeadField = sizeof(uint32_t) + sizeof(uint32_t);

        size_t lenTail, lenTotal;
        CharType * origPtr, * savePtr;

        // Get the original begin address of p.
        origPtr = is.getCurrent() - lenScanned;
        savePtr = is.getCurrent();

        // Find the full length of string
        while (is.peek() != beginToken && is.peek() != _Ch('\0')) {
            is.next();
        }

        // The length of tail characters of string.
        lenTail = is.getCurrent() - savePtr;
        // Get the full length
        lenTotal = lenScanned + lenTail + 1;

        // Allocate the large chunk, and insert it to last.
        jimi_assert(mPoolAllocator != NULL);
        CharType * newCursor = reinterpret_cast<CharType *>(mPoolAllocator->allocateLarge(kSizeOfHeadField + lenTotal * sizeof(CharType)));
        jimi_assert(newCursor != NULL);

        uint32_t * pHeadInfo = reinterpret_cast<uint32_t *>(newCursor);
        *pHeadInfo = static_cast<uint32_t>(kConstStringFlags);
        pHeadInfo++;
        *pHeadInfo = static_cast<uint32_t>(lenTotal);

        // Start copy the string's characters.
        newCursor = reinterpret_cast<CharType *>(pHeadInfo);
        if (is.peek() == beginToken) {
            while (*origPtr != beginToken) {
                *newCursor++ = *origPtr++;
            }
            *newCursor = _Ch('\0');
            is.next();
        }
        else {
            // Error: The tail token is not match, miss quote.
            if (is.peek() == _Ch('\0')) {
                while (*origPtr != _Ch('\0')) {
                    *newCursor++ = *origPtr++;
                }
                *newCursor = _Ch('\0');
            }
            if (isKey)
                this->setError(kKeyStringMissQuoteError);
            else
                this->setError(kValueStringMissQuoteError);
        }
    }

    template <typename InputStreamT, typename ReaderHandlerT>
    ParseResultType parse(InputStreamT & is, ReaderHandlerT & handler) {
        mParseResult.clear();

        jimi_assert(is.peek() != NULL);
        //printf("JsonFx::BasicDocument::parse(const InuptStreamT &) visited.\n\n");
        //setObject();

        while (is.peek() != _Ch('\0')) {
            // Skip the whitespace chars
            skipWhiteSpaces(is);

            if (is.peek() == _Ch('"')) {
                // Parse a string begin from token "
                is.next();
                if (parseFlags & kInsituParseFlag)
                    parseInsituString<_Ch('"')>(is, handler);
                else
                    parseString<_Ch('"')>(is, handler);
            }
            /*
            else if (is.peek() == _Ch('\'')) {
                // Allow use single quotes
                if (parseFlags & kAllowSingleQuotesParseFlag) {
                    // Parse a string begin from token \'
                    is.next();
                    if (parseFlags & kInsituParseFlag)
                        parseInsituString<_Ch('\'')>(is, handler);
                    else
                        parseString<_Ch('\'')>(is, handler);
                }
            }
            //*/
            else if (is.peek() == _Ch('{')) {
                // Parse a object
                is.next();
                parseObject(is, handler);
            }
            else if (is.peek() == _Ch('[')) {
                // Parse a array
                is.next();
                parseArray(is, handler);
            }
            else {
                // Other chars
                is.next();
            }
        }

        return mParseResult;
    }
};

}  // namespace JsonFx

// Define default Reader class type
typedef JsonFx::BasicReader<JsonFx::kDefaultParseFlags, JsonFx::DefaultEncoding, JsonFx::DefaultEncoding>   jfxReader;

#endif  /* !_JSONFX_READER_H_ */
