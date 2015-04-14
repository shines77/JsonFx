
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

    // The space chars including " \t\r\n"
    bool isSpace(const CharType c) const {
        // '\t' = 0x07, '\n' = 0x0A, '\r' = 0x0D
        return (c == CharType(' ')) || (c >= CharType('\t') && c <= CharType('\r'));
    }

    CharType * startObject(CharType * p) {
        return p;
    }

    CharType * startArray(CharType * p) {
        return p;
    }

    CharType * startString(CharType * p, CharType beginToken) {
        // The size of string length field
        static const size_t kSizeOfLengthField = sizeof(uint32_t);
        // Reserve string size
        static const size_t kReserveStringSize = 8;
        jimi_assert(mPoolAllocator != NULL);
        CharType * cursor  = reinterpret_cast<CharType *>(mPoolAllocator->skip(kSizeOfLengthField, kReserveStringSize));
        jimi_assert(cursor != NULL);
        CharType * begin   = cursor;
        CharType * orignal = reinterpret_cast<CharType *>(mPoolAllocator->getChunkCursor());
        CharType * bottom  = reinterpret_cast<CharType *>(mPoolAllocator->getChunkBottom());

        while (*p != beginToken && *p != CharType('\0')) {
            if (cursor < bottom) {
                *cursor = *p;
                ++cursor;
                ++p;
            }
            else {
                CharType * newOrignal = reinterpret_cast<CharType *>(mPoolAllocator->addNewChunk(0));
                CharType * newCursor  = reinterpret_cast<CharType *>(mPoolAllocator->skip(kSizeOfLengthField, kReserveStringSize));
                CharType * newBegin   = newCursor;
                jimi_assert(newCursor != NULL);
                while (begin != cursor) {
                    *newCursor = *begin;
                    ++begin;
                    ++newCursor;
                }
                orignal = newOrignal;
                cursor  = newCursor;
                begin   = newBegin;
                bottom  = reinterpret_cast<CharType *>(mPoolAllocator->getChunkBottom());
                ++cursor;
                ++p;
            }            
        }
        if (*p == beginToken) {
            ++p;
            *cursor = CharType('\0');
            ++cursor;
            jimi_assert(cursor >= begin);
            size_t length = cursor - begin;
            jimi_assert(orignal != NULL);
            *reinterpret_cast<uint32_t *>(orignal) = static_cast<uint32_t>(length);
            mPoolAllocator->allocate(kSizeOfLengthField + length);
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
    //setObject();

    CharType *cur = const_cast<CharType *>(text);
    CharType beginToken;

    while (*cur != CharType('\0')) {
        // Skip space chars
        while (isSpace(*cur))
            ++cur;
        if (*cur == CharType('{')) {
            // Start a object
            ++cur;
            cur = startObject(cur);
        }
        else if (*cur == CharType('"') || *cur == CharType('\'')) {
            // Start a string
            beginToken = *cur;
            ++cur;
            cur = startString(cur, beginToken);
        }
        if (*cur == CharType('[')) {
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
