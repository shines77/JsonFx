
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
    kNoStringEscapeParseFlags   = 1,
    kMaxParseFlags              = 0x80000000U,
    kDefaultParseFlags          = JSONFX_DEFAULT_PARSE_FLAGS
};

// Forward declaration.
template <uint64_t ParseFlags,
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

template <uint64_t ParseFlags,
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
    // The whitespace chars including " \t\n\r"
    template <typename InuptStreamT>
    //JIMI_FORCEINLINE
    inline
    bool isWhiteSpaces(const InuptStreamT & is) const {
        // '\t' = 0x07, '\n' = 0x0A, '\r' = 0x0D
        return ((is.peek() == _Ch(' ')) || (is.peek() >= _Ch('\t') && is.peek() <= _Ch('\r')));
    }

    template <typename InuptStreamT>
    JIMI_FORCEINLINE
    void skipWhiteSpaces(const InuptStreamT & is) {
        // '\t' = 0x07, '\n' = 0x0A, '\r' = 0x0D
        InuptStreamT & nis = const_cast<InuptStreamT &>(is);
#if 1
        while ((nis.peek() == _Ch(' ')) || (nis.peek() >= _Ch('\t') && nis.peek() <= _Ch('\r'))) {
            nis.next();
        }
#else
        while (isWhiteSpaces(nis)) {
            nis.next();
        }
#endif
    }

    CharType * startObject(CharType *p) {
        return p;
    }

    CharType * startArray(CharType *p) {
        return p;
    }

    template <CharType beginToken>
    CharType * startString(CharType * p) {
        jimi_assert(mPoolAllocator != NULL);
        // The size of string length field
        static const size_t kSizeOfHeadField = sizeof(uint32_t) + sizeof(uint32_t);
        // Reserve string size
        static const size_t kReserveStringSize = 8;
        CharType * cursor  = reinterpret_cast<CharType *>(mPoolAllocator->skip(kSizeOfHeadField, kReserveStringSize));
        jimi_assert(cursor != NULL);
        CharType * begin   = cursor;
        CharType * bottom  = reinterpret_cast<CharType *>(mPoolAllocator->getChunkBottom());

        while (*p != beginToken && *p != _Ch('\0')) {
            if (cursor < bottom) {
                *cursor++ = *p++;
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

                while (*p != beginToken && *p != _Ch('\0')) {
                    if (cursor < bottom) {
                        *cursor++ = *p++;
                    }
                    else {
                        // If it need allocate memory second time, mean the string's length
                        // is more than PoolAllocator's kChunkCapacoty bytes, so we find
                        // out the string's length first, and allocate the enough memory
                        // to fill the string's characters.
                        size_t lenScanned = cursor - begin;
                        p = startLargeString<beginToken>(p, lenScanned);
                        return p;
                    }
                }
            }
        }
        // It's the ending of string token.
        if (*p == beginToken) {
            ++p;
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
            //mErrno = -1;
        }
        return p;
    }

    template <CharType beginToken>
    JIMI_NOINLINE_DECLARE(CharType *) startLargeString(CharType *p, size_t lenScanned) {
        // The size of string length field
        static const size_t kSizeOfHeadField = sizeof(uint32_t) + sizeof(uint32_t);

        size_t lenTail, lenTotal;
        CharType *origPtr, *savePtr;

        // Get the original begin address of p.
        origPtr = p - lenScanned;
        savePtr = p;

        // Find the full length of string
        while (*p != beginToken && *p != _Ch('\0')) {
            ++p;
        }

        // The length of tail characters of string.
        lenTail = p - savePtr;
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
        if (*p == beginToken) {
            while (*origPtr != beginToken) {
                *newCursor++ = *origPtr++;
            }
            *newCursor = _Ch('\0');
            ++p;
        }
        else {
            // Error: The tail token is not match.
            if (*p == _Ch('\0')) {
                while (*origPtr != _Ch('\0')) {
                    *newCursor++ = *origPtr++;
                }
                *newCursor = _Ch('\0');
            }
            //mErrno = -1;
        }
        return p;
    }

    template <typename InputStreamT, typename ReaderHandlerT>
    ParseResultType parse(const InputStreamT & is, const ReaderHandlerT & handler) {
        mParseResult.clear();

        jimi_assert(is.peek() != NULL);
        //printf("JsonFx::BasicDocument::parse(const InuptStreamT &) visited.\n\n");
        //setObject();

        CharType *cur = const_cast<CharType *>(is.getCurrent());
        CharType beginToken;

        while (*cur != _Ch('\0')) {
            // Skip the whitespace chars
            skipWhiteSpaces(is);

            if (*cur == _Ch('"')) {
                // Start a string begin from token "
                beginToken = *cur;
                ++cur;
                cur = startString<_Ch('"')>(cur);
            }
#if 0
            else if (*cur == _Ch('\'')) {
                // Start a string begin from token \'
                beginToken = *cur;
                ++cur;
                cur = startString<_Ch('\'')>(cur);
            }
#endif
            if (*cur == _Ch('{')) {
                // Start a object
                ++cur;
                cur = startObject(cur);
            }
            if (*cur == _Ch('[')) {
                // Start a array
                ++cur;
                cur = startArray(cur);
            }
            else {
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
