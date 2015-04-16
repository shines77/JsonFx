
#ifndef _JSONFX_DOCUMENT_H_
#define _JSONFX_DOCUMENT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Config.h"
#include "JsonFx/CharSet.h"
#include "JsonFx/Allocator.h"
#include "JsonFx/Value.h"

#include "jimi/basic/assert.h"

namespace JsonFx {

template <typename Encoding = JSONFX_DEFAULT_ENCODING,
          typename PoolAllocator = DefaultPoolAllocator,
          typename Allocator = DefaultAllocator>
class BasicDocument : public BasicValue<Encoding, PoolAllocator>
{
public:
    typedef typename Encoding::CharType         CharType;           //!< Character type derived from Encoding.
    typedef BasicValue<Encoding, PoolAllocator> ValueType;          //!< Value type of the document.
    typedef Encoding                            EncodingType;       //!< Character encoding type.
    typedef PoolAllocator                       PoolAllocatorType;  //!< Pool allocator type from template parameter.
    typedef Allocator                           AllocatorType;      //!< Stack allocator type from template parameter.

private:    
    PoolAllocatorType * mPoolAllocator;
    bool                mPoolAllocatorNeedFree;

    int         mErrno;
    int         mErrLine;
    int         nErrOffset;

public:
    BasicDocument(PoolAllocatorType * poolAllocator = NULL)
        : mPoolAllocator(poolAllocator), mPoolAllocatorNeedFree(false)
        , mErrno(0), mErrLine(-1), nErrOffset(-1)
    {
        initPoolAllocator(poolAllocator);
    }

    ~BasicDocument() {
        destroy();
    }

private:
    //! Prohibit copying
    BasicDocument(const BasicDocument &);
    //! Prohibit assignment
    BasicDocument & operator =(const BasicDocument &);

public:
    PoolAllocatorType * getAllocator() const { return mPoolAllocator; }

    BasicDocument & parse(const CharType * text);

    void visit();

    void test() {
        printf("JsonFx::BasicDocument::test()\n\n");
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

    void initPoolAllocator(PoolAllocatorType * poolAllocator) {
        if (poolAllocator == NULL) {
            PoolAllocatorType * newPoolAllocator = new PoolAllocatorType();
            jimi_assert(newPoolAllocator != NULL);
            if (newPoolAllocator != NULL) {
                mPoolAllocator = newPoolAllocator;
                mPoolAllocatorNeedFree = true;
            }
        }
    }

    // The space chars including " \t\n\r"
    bool isSpace(const CharType c) const {
        // '\t' = 0x07, '\n' = 0x0A, '\r' = 0x0D
        return (c == _Ch(' ')) || (c >= _Ch('\t') && c <= _Ch('\r'));
    }

    CharType * startObject(CharType * p) {
        return p;
    }

    CharType * startArray(CharType * p) {
        return p;
    }

    template <CharType beginToken>
    CharType * startString(CharType * p) {
        jimi_assert(mPoolAllocator != NULL);
        // The size of string length field
        static const size_t kSizeOfHeadField = sizeof(uint32_t) + sizeof(uint32_t);
        // Reserve string size
        static const size_t kReserveStringSize = 8;
        CharType * cursor  = reinterpret_cast<CharType *>(mPoolAllocator->reserve(kSizeOfHeadField, kReserveStringSize));
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
                CharType * newCursor = reinterpret_cast<CharType *>(mPoolAllocator->fastReserve(kSizeOfHeadField, kReserveStringSize));
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
                        size_t lenNow = cursor - begin;
                        p = startLargeString<beginToken>(p, lenNow);
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
            mErrno = -1;
        }
        return p;
    }

    template <CharType beginToken>
    CharType * startLargeString(CharType * p, size_t lenNow) {
        // The size of string length field
        static const size_t kSizeOfHeadField = sizeof(uint32_t) + sizeof(uint32_t);

        size_t lenTail, lenTotal;
        CharType *origPtr, *savePtr;

        // Get the original begin address of p.
        origPtr = p - lenNow;
        savePtr = p;

        // Find the full length of string
        while (*p != beginToken && *p != _Ch('\0')) {
            ++p;
        }

        // The length of tail characters of string.
        lenTail = p - savePtr;
        // Get the full length
        lenTotal = lenNow + lenTail + 1;

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
            mErrno = -1;
        }
        return p;
    }
};

// Define default Document class type
typedef BasicDocument<>     Document;

template <typename Encoding, typename PoolAllocator, typename Allocator>
void BasicDocument<Encoding, PoolAllocator, Allocator>::visit()
{
    printf("JsonFx::BasicDocument::visit(). EncodingType = %d\n\n", EncodingType::type);
}

template <typename Encoding, typename PoolAllocator, typename Allocator>
BasicDocument<Encoding, PoolAllocator, Allocator> &
BasicDocument<Encoding, PoolAllocator, Allocator>::parse(const CharType * text)
{
    jimi_assert(text != NULL);
    //printf("JsonFx::BasicDocument::parse() visited.\n\n");
    setObject();

    CharType *cur = const_cast<CharType *>(text);
    CharType beginToken;

    while (*cur != _Ch('\0')) {
        // Skip space chars
        while (isSpace(*cur))
            ++cur;
        if (*cur == _Ch('{')) {
            // Start a object
            ++cur;
            cur = startObject(cur);
        }
        else if (*cur == _Ch('"')) {
            // Start a string begin from token "
            beginToken = *cur;
            ++cur;
            cur = startString<_Ch('"')>(cur);
        }
        else if (*cur == _Ch('\'')) {
            // Start a string begin from token \'
            beginToken = *cur;
            ++cur;
            cur = startString<_Ch('\'')>(cur);
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

    return *this;
}

}  // namespace JsonFx

#endif  /* !_JSONFX_DOCUMENT_H_ */
