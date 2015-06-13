
#ifndef _JSONFX_READER_H_
#define _JSONFX_READER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Allocator.h"
#include "JsonFx/Stack.h"
#include "JsonFx/ParseResult.h"
#include "JsonFx/Stream/StringInputStream.h"
#include "JsonFx/Internal/Traits.h"

#define JSONFX_DEFAULT_PARSE_FLAGS      (kNoneParseFlag)

namespace JsonFx {

enum ParseFlags {
    kNoneParseFlag                  = 0,
    kInsituParseFlag                = 1 << 0,
    kNoStringEscapeParseFlags       = 1 << 8,
    kAllowSingleQuotesParseFlag     = 1 << 9,
    kMaxParseFlags                  = 0x80000000U,
    kDefaultParseFlags              = JSONFX_DEFAULT_PARSE_FLAGS
};

// Forward declaration.
template <size_t parseFlags,
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

template <size_t parseFlags,
          typename SourceEncodingT,
          typename TargetEncodingT,
          typename PoolAllocatorT /* = DefaultPoolAllocator */,
          typename StackAllocatorT /* = DefaultAllocator */>
class BasicReader {
public:
    typedef typename SourceEncodingT::CharType  CharType;   //!< SourceEncoding character type
    typedef BasicStringInputStream<CharType>    StringInputStreamType;
    typedef PoolAllocatorT                      PoolAllocatorType;  //!< Pool allocator type from template parameter.
    typedef StackAllocatorT                     StackAllocatorType; //!< Stack allocator type from template parameter.
    typedef BasicStack<PoolAllocatorT>          StackType;
    typedef BasicParseResult<SourceEncodingT>   ParseResultType;

    static const size_t kDefaultStackCapacity = 256;

private:
    StringInputStreamType * mInputStream;
    PoolAllocatorType *     mPoolAllocator;
    bool                    mPoolAllocatorNeedFree;
    StackType               mStack;
    ParseResultType         mParseResult;

public:
    BasicReader(const PoolAllocatorType * poolAllocator = NULL,
                bool poolAllocatorNeedFree = false,
                const PoolAllocatorType * stackAllocator = NULL,
                size_t stackCapacity = kDefaultStackCapacity)
        : mInputStream(NULL), mPoolAllocator(const_cast<PoolAllocatorType *>(poolAllocator)),
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
    bool hasParseError() const { return mParseResult.hasError(); }

    ParseErrorCode getParseError() const {
        return mParseResult.getError();
    }

    void setParseError(ParseErrorCode code) {
        mParseResult.setError(code);
    }

    void setParseError(ParseErrorCode code, size_t offset) {
        mParseResult.setError(code, offset);
    }

    // The whitespace chars including " \t\n\r"
    template <typename InuptStreamT>
    JIMI_FORCEINLINE
    bool isWhiteSpaces(const InuptStreamT & is) const {
        // '\t' = 0x07, '\n' = 0x0A, '\r' = 0x0D
        return ((is.peek() == ' ') || (is.peek() >= '\t' && is.peek() <= '\r'));
    }

    template <typename InuptStreamT>
    JIMI_FORCEINLINE
    void skipWhiteSpaces(InuptStreamT & is) {
        // '\t' = 0x07, '\n' = 0x0A, '\r' = 0x0D
#if 1
        while ((is.peek() == ' ') || (is.peek() >= '\t' && is.peek() <= '\r')) {
            is.next();
        }
#else
        while (isWhiteSpaces(is)) {
            is.next();
        }
#endif
    }

    // The whitespace chars including " \t\n\r"
    JIMI_FORCEINLINE
    bool isWhiteSpaces(const CharType * src) const {
        // '\t' = 0x07, '\n' = 0x0A, '\r' = 0x0D
        return ((*src == ' ') || (*src >= '\t' && *src <= '\r'));
    }

    JIMI_FORCEINLINE
    const CharType * skipWhiteSpaces(const CharType * src) {
        // '\t' = 0x07, '\n' = 0x0A, '\r' = 0x0D
        while ((*src == ' ') || (*src >= '\t' && *src <= '\r')) {
            ++src;
        }
        return src;
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

    template <CharType quoteToken, typename InputStreamT, typename ReaderHandlerT>
    void parseString(InputStreamT & is, ReaderHandlerT & handler, bool isKey = true) {
        jimi_assert(mPoolAllocator != NULL);
        // The size of string length field
        static const size_t kSizeOfHeadField = sizeof(uint32_t) + sizeof(uint32_t);
        // Reserve string size
        static const size_t kReserveStringSize = 8;
        CharType * cursor = (CharType *)mPoolAllocator->skip(kSizeOfHeadField, kReserveStringSize);
        jimi_assert(cursor != NULL);
        CharType * begin  = cursor;
        CharType * bottom = (CharType *)mPoolAllocator->getChunkBottom();

        while (is.peek() != quoteToken && is.peek() != '\0') {
            if (cursor < bottom) {
                *cursor++ = is.take();
            }
            else {
                // The remain space in the active chunk is not enough to store the string's
                // characters, so we allocate a new chunk to store it.
                CharType * newCursor = (CharType *)mPoolAllocator->addNewChunkAndSkip(kSizeOfHeadField, kReserveStringSize);
                CharType * newBegin  = newCursor;
                jimi_assert(newCursor != NULL);
                while (begin != cursor) {
                    *newCursor++ = *begin++;
                }
                cursor = newCursor;
                begin  = newBegin;
                bottom = (CharType *)mPoolAllocator->getChunkBottom();

                while (is.peek() != quoteToken && is.peek() != '\0') {
                    if (cursor < bottom) {
                        *cursor++ = is.take();
                    }
                    else {
                        // If it need allocate memory second time, mean the string's length
                        // is more than PoolAllocator's kChunkCapacoty bytes, so we find
                        // out the string's length first, and allocate the enough memory
                        // to fill the string's characters.
                        size_t lenScanned = cursor - begin;
                        this->parseLargeString<quoteToken>(is, handler, isKey, lenScanned);
                    }
                }
            }
        }
        // It's the ending of string token.
        if (is.peek() == quoteToken) {
            is.next();
            *cursor = '\0';
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
                this->setParseError(kKeyStringMissQuoteError);
            else
                this->setParseError(kValueStringMissQuoteError);
        }
    }

    template <CharType quoteToken, typename InputStreamT, typename ReaderHandlerT>
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
        while (is.peek() != quoteToken && is.peek() != '\0') {
            is.next();
        }

        // The length of tail characters of string.
        lenTail = is.getCurrent() - savePtr;
        // Get the full length
        lenTotal = lenScanned + lenTail + 1;

        // Allocate the large chunk, and insert it to last.
        jimi_assert(mPoolAllocator != NULL);
        CharType * newCursor = (CharType *)mPoolAllocator->allocateLarge(kSizeOfHeadField + lenTotal * sizeof(CharType));
        jimi_assert(newCursor != NULL);

        uint32_t * pHeadInfo = reinterpret_cast<uint32_t *>(newCursor);
        *pHeadInfo = static_cast<uint32_t>(kConstStringFlags);
        pHeadInfo++;
        *pHeadInfo = static_cast<uint32_t>(lenTotal);

        // Start copy the string's characters.
        newCursor = reinterpret_cast<CharType *>(pHeadInfo);
        if (is.peek() == quoteToken) {
            while (*origPtr != quoteToken) {
                *newCursor++ = *origPtr++;
            }
            *newCursor = '\0';
            is.next();
        }
        else {
            // Error: The tail token is not match, miss quote.
            if (is.peek() == '\0') {
                while (*origPtr != '\0') {
                    *newCursor++ = *origPtr++;
                }
                *newCursor = '\0';
            }
            if (isKey)
                this->setParseError(kKeyStringMissQuoteError);
            else
                this->setParseError(kValueStringMissQuoteError);
        }
    }

    template <typename InputStreamT, typename ReaderHandlerT>
    ParseResultType parseStream(InputStreamT & is, ReaderHandlerT & handler) {
        mParseResult.clear();

        jimi_assert(is.peek() != NULL);
        //printf("JsonFx::BasicDocument::parse(const InuptStreamT &) visited.\n\n");
        //setObject();

        while (is.peek() != '\0') {
            // Skip the whitespace chars
            skipWhiteSpaces(is);

            if (is.peek() == '"') {
                // Parse a string begin from token ["]
                is.next();
                if (parseFlags & kInsituParseFlag)
                    parseInsituString<'"'>(is, handler);
                else
                    parseString<'"'>(is, handler);
            }
            else if (is.peek() == '\'') {
                // Allow use the single quotes [\']
                if (parseFlags & kAllowSingleQuotesParseFlag) {
                    // Parse a string begin from token \'
                    is.next();
                    if (parseFlags & kInsituParseFlag)
                        parseInsituString<'\''>(is, handler);
                    else
                        parseString<'\''>(is, handler);
                }
            }
            else if (is.peek() == '{') {
                // Parse a object
                is.next();
                parseObject(is, handler);
            }
            else if (is.peek() == '[') {
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

    template <CharType quoteToken, typename ReaderHandlerT>
    const CharType * parseInsituString(const CharType * src, ReaderHandlerT & handler, bool isKey = true) {
        return src;
    }

#if 0
    JIMI_FORCEINLINE
    unsigned parseHex4(const CharType * src) {
        const CharType * end = src + 4;
        unsigned codepoint = 0;
        do {
            CharType c = *src;
            if (c >= '0' && c <= '9') {
                c -= '0';
            }
            else {
                c &= (CharType)(~ (CharType)' ');
                if (c >= 'A' && c <= 'F') {
                    c -= 'A' - 10;
                }
                else {
                    this->setParseError(kStringUnicodeEscapeInvalidHexError, this->tell(src));
                    return 0;
                }
            }
            codepoint <<= 4;
            codepoint |= static_cast<unsigned>(c);
            ++src;
        } while (src != end);
        return codepoint;
    }
#else
    JIMI_FORCEINLINE
    unsigned parseHex4(const CharType * &src) {
        const CharType * end = src + 4;
        unsigned codepoint = 0;
        do {
            unsigned c = (unsigned)(*src);
            if (c >= '0' && c <= '9') {
                c -= '0';
            }
            else {
                c &= (unsigned)(~ (unsigned)' ');
                if (c >= 'A' && c <= 'F') {
                    c -= 'A' - 10;
                }
                else {
                    this->setParseError(kStringUnicodeEscapeInvalidHexError, this->tell(src));
                    return 0;
                }
            }
            codepoint <<= 4;
            codepoint |= static_cast<unsigned>(c);
            ++src;
        } while (src != end);
        return codepoint;
    }
#endif

    JIMI_FORCEINLINE
    size_t tell(const CharType * src) const {
        jimi_assert(mInputStream != NULL);
        jimi_assert(src >= mInputStream->getBegin());
        return static_cast<size_t>(src - mInputStream->getBegin());
    }

    JIMI_FORCEINLINE
    void unescapeUnicode(CharType * &dest, const CharType * &src) {
        ++src;
        unsigned codepoint = parseHex4(src);
        //src += 4;
        if (codepoint >= 0xD800 && codepoint <= 0xDBFF) {
            // Handle UTF-16 surrogate pair.
            if (*src++ != '\\' || *src++ != 'u') {
                this->setParseError(kStringUnicodeSurrogateInvalidError, this->tell(src) - 2);
            }
            unsigned codepoint2 = parseHex4(src);
            //src += 4;
            if (codepoint2 < 0xDC00 || codepoint2 > 0xDFFF)
                this->setParseError(kStringUnicodeSurrogateInvalidError, this->tell(src) - 2);
            codepoint = (((codepoint - 0xD800) << 10) | (codepoint2 - 0xDC00)) + 0x10000;
        }
        //TargetEncodingT::encode(dest, codepoint);                    
        *dest++ = 'X';
        *dest++ = 'X';
        *dest++ = 'X';
#if 0
        static int count = 0;
        if (count == 0)
            printf("codepoint = 0x%08X\n", codepoint);
        count++;
#endif
    }

    JIMI_FORCEINLINE
    void unescapeChars(CharType * &dest, const CharType * &src) {
        // Skip the '\\' char.
        ++src;

        if (sizeof(CharType) == 1 || unsigned(*src) < 256) {
            // Unescape the '\r', '\n', '\t', '\b', '\f', '/', '\\', '"' chars.
            if (*src <= 't' && *src >= '"') {
                if (*src == 't')
                    *dest++ = '\t';
                else if (*src == '"')
                    *dest++ = '"';
                else if (*src == 'n')
                    *dest++ = '\n';
                else if (*src == 'r')
                    *dest++ = '\r';
                else if (*src == '\\')
                    *dest++ = '\\';
                else if (*src == '/')
                    *dest++ = '/';
                else if (*src == 'b')
                    *dest++ = '\b';
                else if (*src == 'f')
                    *dest++ = '\f';
                else {
                    // Unknown escape chars, do nothing.
                    --src;
                }
                ++src;
            }
            else if (*src == 'u') {
                // '\uXXXX'
#if 0
                unescapeUnicode(dest, src);
#else
                ++src;
                unsigned codepoint = parseHex4(src);
                //src += 4;
                if (codepoint >= 0xD800 && codepoint <= 0xDBFF) {
                    // Handle UTF-16 surrogate pair.
                    if (*src++ != '\\' || *src++ != 'u') {
                        this->setParseError(kStringUnicodeSurrogateInvalidError, this->tell(src) - 2);
                    }
                    unsigned codepoint2 = parseHex4(src);
                    //src += 4;
                    if (codepoint2 < 0xDC00 || codepoint2 > 0xDFFF)
                        this->setParseError(kStringUnicodeSurrogateInvalidError, this->tell(src) - 2);
                    codepoint = (((codepoint - 0xD800) << 10) | (codepoint2 - 0xDC00)) + 0x10000;
                }
                //TargetEncodingT::encode(dest, codepoint);                    
                *dest++ = 'X';
                *dest++ = 'X';
                *dest++ = 'X';
#if 0
                static int count = 0;
                if (count == 0)
                    printf("codepoint = 0x%08X\n", codepoint);
                count++;
#endif
#endif
            }
            else {
                // Unknown escape chars, do nothing.
            }
        }
    }

    JIMI_FORCEINLINE
    void calcUnescapeCharBytes(const CharType * &src, int & additive) {
        // Skip the '\\' char.
        ++src;

        if (sizeof(CharType) == 1 || unsigned(*src) < 256) {
            // Unescape the '\r', '\n', '\t', '\b', '\f', '/', '\\', '"' chars.
            if (*src <= 't' && *src >= '"') {
                if (   *src == 't'  || *src == '"' || *src == 'n'
                    || *src == 'r'  || *src == '\\' || *src == '/'
                    || *src == 'b'  || *src == 'f')
                {
                    src++;
                    additive--;
                }
                else if (*src == 'u') {
                    // '\uXXXX'
                    src += 5;
                    // Needn't subtract the additive, for efficiency.
                    // additive -= 5 - 3;   // A utf-8 multi-language unicode character is 3 bytes.
                }
                else {
                    // Unknown escape chars.
                }
            }
            else {
                // Unknown escape chars.
            }
        }
    }

    template <CharType quoteToken, typename ReaderHandlerT>
    const CharType * parseString(const CharType * src, ReaderHandlerT & handler, bool isKey = true) {
        jimi_assert(mPoolAllocator != NULL);
        // The size of string length field
        static const size_t kSizeOfHeadField = sizeof(uint32_t) + sizeof(uint32_t);
        // Reserve string size
        static const size_t kReserveStringSize = 8;
        CharType * cursor = (CharType *)mPoolAllocator->skip(kSizeOfHeadField, kReserveStringSize);
        jimi_assert(cursor != NULL);
        CharType * begin  = cursor;
        CharType * bottom = (CharType *)mPoolAllocator->getChunkBottom();

        while (*src != quoteToken && *src != '\0') {
            if (cursor < bottom) {
                if (*src != '\\')
                    *cursor++ = *src++;
                else
                    this->unescapeChars(cursor, src);
            }
            else {
                // The remain space in the active chunk is not enough to store the string's
                // characters, so we allocate a new chunk to store it.
                CharType * newCursor = (CharType *)mPoolAllocator->addNewChunkAndSkip(kSizeOfHeadField, kReserveStringSize);
                CharType * newBegin  = newCursor;
                jimi_assert(newCursor != NULL);
                // Copy previous parse strings.
                while (begin != cursor) {
                    *newCursor++ = *begin++;
                }
                cursor = newCursor;
                begin  = newBegin;
                bottom = (CharType *)mPoolAllocator->getChunkBottom();

                while (*src != quoteToken && *src != '\0') {
                    if (cursor < bottom) {
                        if (*src != '\\')
                            *cursor++ = *src++;
                        else
                            this->unescapeChars(cursor, src);
                    }
                    else {
                        // If it need allocate memory second time, mean the string's length
                        // is more than PoolAllocator's kChunkCapacoty bytes, so we find
                        // out the string's length first, and allocate the enough memory
                        // to fill the string's characters.
                        size_t lenScanned = cursor - begin;
                        src = this->parseLargeString<quoteToken>(src, handler, isKey, lenScanned);
                        return src;
                    }
                }
            }
        }

        // It's the ending of string token.
        if (*src == quoteToken) {
            ++src;
            *cursor = '\0';
            ++cursor;
            jimi_assert(cursor >= begin);
            size_t length = cursor - begin;
            uint32_t * pHeadInfo = reinterpret_cast<uint32_t *>(reinterpret_cast<char *>(begin) - kSizeOfHeadField);
            jimi_assert(pHeadInfo != NULL);
            *pHeadInfo = static_cast<uint32_t>(kConstStringFlags);
            pHeadInfo++;
            *pHeadInfo = static_cast<uint32_t>(length);
            mPoolAllocator->allocate(kSizeOfHeadField + length * sizeof(CharType));
            return src;
        }
        else {
            // Error: The tail token is not match.
            if (isKey)
                this->setParseError(kKeyStringMissQuoteError);
            else
                this->setParseError(kValueStringMissQuoteError);
            return src;
        }
    }

    template <CharType quoteToken, typename ReaderHandlerT>
    JIMI_NOINLINE_DECLARE(const CharType *)
    parseLargeString(const CharType * src, ReaderHandlerT & handler,
                     bool isKey, size_t lenScanned) {
        // The size of string length field
        static const size_t kSizeOfHeadField = sizeof(uint32_t) + sizeof(uint32_t);

        size_t lenTail, lenTotal;
        int lenAdditive;
        const CharType * origPtr, * savePtr;

        // Get the original begin address of p.
        origPtr = src - lenScanned;
        savePtr = src;

        // The escape chars additive length.
        lenAdditive = 0;

        // Find the full length of string
        while (*src != quoteToken && *src != '\0') {
#if 1
            ++src;
#else
            if (*src != '\\')
                ++src;
            else
                this->calcUnescapeCharBytes(src, lenAdditive);
#endif
        }

        // lenAdditive must be <= 0.
        jimi_assert(lenAdditive <= 0);

        // The length of tail characters of string.
        lenTail = src - savePtr;
        // Get the full length
        lenTotal = lenScanned + lenTail + lenAdditive + 1;

        // Allocate the large chunk, and insert it to last.
        jimi_assert(mPoolAllocator != NULL);
        CharType * newCursor = (CharType *)mPoolAllocator->allocateLarge(kSizeOfHeadField + lenTotal * sizeof(CharType));
        jimi_assert(newCursor != NULL);

        uint32_t * pHeadInfo = reinterpret_cast<uint32_t *>(newCursor);
        *pHeadInfo = static_cast<uint32_t>(kConstStringFlags);
        pHeadInfo++;
        *pHeadInfo = static_cast<uint32_t>(lenTotal);

        // Start copy the string's characters.
        newCursor = reinterpret_cast<CharType *>(pHeadInfo);
        if (*src == quoteToken) {
            while (*origPtr != quoteToken) {
                if (*origPtr != '\\')
                    *newCursor++ = *origPtr++;
                else
                    this->unescapeChars(newCursor, origPtr);
            }
            *newCursor = '\0';
            ++src;
            return src;
        }
        else {
            // Error: The tail token is not match, miss quote.
            if (*src == '\0') {
                while (*origPtr != '\0') {
                    *newCursor++ = *origPtr++;
                }
                *newCursor = '\0';
            }
            if (isKey)
                this->setParseError(kKeyStringMissQuoteError);
            else
                this->setParseError(kValueStringMissQuoteError);
            return src;
        }
    }

    template <typename InputStreamT, typename ReaderHandlerT>
    ParseResultType parse(const InputStreamT & is, const ReaderHandlerT & handler) {
        mParseResult.clear();

        jimi_assert(is.peek() != NULL);
        //printf("JsonFx::BasicDocument::parse(const InuptStreamT &) visited.\n\n");
        //setObject();

        mInputStream = &(const_cast<InputStreamT &>(is));

        const CharType * cur = is.getCurrent();
        while (*cur != '\0') {
            // Skip the whitespace chars
            cur = skipWhiteSpaces(cur);
            //skipWhiteSpaces(is);

            if (*cur == '"') {
                // Parse a string begin from token ["]
                ++cur;
                if (!(parseFlags & kInsituParseFlag))
                    cur = parseString<'"'>(cur, handler);
                else
                    cur = parseInsituString<'"'>(cur, handler);
            }
            else if (*cur == '\'') {
                // Allow use the single quotes [\']
                if (parseFlags & kAllowSingleQuotesParseFlag) {
                    // Parse a string begin from token [\']
                    ++cur;
                    if (!(parseFlags & kInsituParseFlag))
                        parseString<'\''>(cur, handler);
                    else
                        parseInsituString<'\''>(cur, handler);
                }
            }
            else if (*cur == '{') {
                // Parse a object
                ++cur;
                parseObject(is, handler);
            }
            else if (*cur == '[') {
                // Parse a array
                ++cur;
                parseArray(is, handler);
            }
            else {
                // Other chars
                ++cur;
            }
        }
        return mParseResult;
    }
};

}  // namespace JsonFx

// Define default Reader class type
typedef JsonFx::BasicReader<JsonFx::kDefaultParseFlags, JsonFx::DefaultEncoding, JsonFx::DefaultEncoding>   jfxReader;

#endif  /* !_JSONFX_READER_H_ */
