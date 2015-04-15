
#ifndef _JSONFX_DOCUMENT_H_
#define _JSONFX_DOCUMENT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>
#include <tchar.h>

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

    CharType * startString(CharType * p, CharType beginToken) {
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
                CharType * newCursor = reinterpret_cast<CharType *>(mPoolAllocator->fastReserve(kSizeOfHeadField, kReserveStringSize));
                CharType * newBegin  = newCursor;
                jimi_assert(newCursor != NULL);
                while (begin != cursor) {
                    *newCursor++ = *begin++;
                }
                cursor  = newCursor;
                begin   = newBegin;
                bottom  = reinterpret_cast<CharType *>(mPoolAllocator->getChunkBottom());
            }            
        }
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
            mPoolAllocator->allocate(kSizeOfHeadField + length);
        }
        else {
            mErrno = -1;
        }
        return p;
    }

private:    
    PoolAllocatorType * mPoolAllocator;
    bool                mPoolAllocatorNeedFree;

    int         mErrno;
    int         mErrLine;
    int         nErrOffset;
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
        else if (*cur == _Ch('"') || *cur == _Ch('\'')) {
            // Start a string
            beginToken = *cur;
            ++cur;
            cur = startString(cur, beginToken);
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
