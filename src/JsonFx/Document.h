
#ifndef _JSONFX_DOCUMENT_H_
#define _JSONFX_DOCUMENT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Config.h"
#include "JsonFx/Allocator.h"
#include "JsonFx/Value.h"
#include "JsonFx/Reader.h"
#include "JsonFx/Writer.h"

#include "JsonFx/Stream/StringInputStream.h"

#include "jimi/basic/assert.h"

namespace JsonFx {

// Forward declaration.
template <typename EncodingT = JSONFX_DEFAULT_ENCODING,
          typename PoolAllocatorT = DefaultPoolAllocator,
          typename AllocatorT = DefaultAllocator>
class BasicDocument;

// Define default Document class type
typedef BasicDocument<>    Document;

// Save and setting the packing alignment
#pragma pack(push)
#pragma pack(1)

template <typename EncodingT /* = JSONFX_DEFAULT_ENCODING */,
          typename PoolAllocatorT /* = DefaultPoolAllocator */,
          typename AllocatorT /* = DefaultAllocator */>
class BasicDocument : public BasicValue<EncodingT, PoolAllocatorT>
{
public:
    typedef typename EncodingT::CharType            CharType;           //!< Character type derived from Encoding.
    typedef BasicValue<EncodingT, PoolAllocatorT>   ValueType;          //!< Value type of the document.
    typedef EncodingT                               EncodingType;       //!< Character encoding type.
    typedef PoolAllocatorT                          PoolAllocatorType;  //!< Pool allocator type from template parameter.
    typedef AllocatorT                              AllocatorType;      //!< Stack allocator type from template parameter.
    typedef BasicStringInputStream<CharType>        StringInputStreamT;

private:    
    PoolAllocatorType * mPoolAllocator;
    bool                mPoolAllocatorNeedFree;

public:
    BasicDocument(PoolAllocatorType * poolAllocator = NULL)
        : mPoolAllocator(poolAllocator), mPoolAllocatorNeedFree(false)
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

    // The whitespace chars including " \t\n\r"
    template <typename InuptStreamT>
    //JIMI_FORCEINLINE
    inline
    bool isWhiteSpaces(InuptStreamT & is) const {
        // '\t' = 0x07, '\n' = 0x0A, '\r' = 0x0D
        return (bool)((is.peek() == _Ch(' ')) || (is.peek() >= _Ch('\t') && is.peek() <= _Ch('\r')));
    }

    template <typename InuptStreamT>
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

public:
    //
    // BasicDocument::parse<parseFlags, SourceEncodingT, InuptStreamT>(is);
    //
    template <uint64_t parseFlags, typename SourceEncodingT, typename InuptStreamT>
    BasicDocument & parse(InuptStreamT & is) {
        // Remove existing root if exist
        ValueType::setNull();
        BasicReader<SourceEncodingT, EncodingT, PoolAllocatorT> reader(this->getAllocator());

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

        return *this;
    }

    //
    // BasicDocument::parse(InputStreamT & is);
    //
    template <uint64_t parseFlags, typename InputStreamT>
    BasicDocument & parse(InputStreamT & is) {
        return parse<parseFlags, EncodingT, InputStreamT>(is);
    }

    template <typename InputStreamT>
    BasicDocument & parse(InputStreamT & is) {
        return parse<kDefaultParseFlags>(is);
    }

    //
    // BasicDocument::parse(StringInputStreamT & is);
    //
    template <uint64_t parseFlags, typename SourceEncodingT>
    BasicDocument & parse(StringInputStreamT & is) {
        return parse<parseFlags, SourceEncodingT, StringInputStreamT>(is);
    }

    template <uint64_t parseFlags>
    BasicDocument & parse(StringInputStreamT & is) {
        return parse<parseFlags, EncodingT>(is);
    }

    BasicDocument & parse(StringInputStreamT & is) {
        return parse<kDefaultParseFlags>(is);
    }

    //
    // BasicDocument::parse(const CharType * text);
    //
    template <uint64_t parseFlags, typename SourceEncodingT, typename InputStreamT>
    BasicDocument & parse(const CharType * text) {
        jimi_assert(text != NULL);
        InputStreamT inputStream(text);
        return parse<parseFlags, SourceEncodingT, InputStreamT>(inputStream);
    }

    template <uint64_t parseFlags, typename SourceEncodingT>
    BasicDocument & parse(const CharType * text) {
        return parse<parseFlags, SourceEncodingT, StringInputStreamT>(text);
    }

    template <typename InputStreamT>
    BasicDocument & parse(const typename InputStreamT::CharType * text) {
        return parse<kDefaultParseFlags, EncodingT, InputStreamT>(text);
    }

    template <uint64_t parseFlags>
    BasicDocument & parse(const CharType * text) {
        return parse<parseFlags, EncodingT>(text);
    }

    BasicDocument & parse(const CharType * text) {
        return parse<kDefaultParseFlags>(text);
    }
};

// Recover the packing alignment
#pragma pack(pop)

template <typename EncodingT, typename PoolAllocatorT, typename AllocatorT>
void BasicDocument<EncodingT, PoolAllocatorT, AllocatorT>::visit()
{
    printf("JsonFx::BasicDocument::visit(). EncodingType = %d\n\n", EncodingType::type);
}

}  // namespace JsonFx

// Define default Document class type
typedef JsonFx::BasicDocument<>     jfxDocument;

#endif  /* !_JSONFX_DOCUMENT_H_ */
