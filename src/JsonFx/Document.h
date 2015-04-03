
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
          typename Allocator = DefaultPoolAllocator,
          typename StackAllocator = CrtAllocator>
class BasicDocument : public BasicValue<Encoding, Allocator>
{
public:
    typedef typename Encoding::CharType     CharType;       //!< Character type derived from Encoding.
    typedef BasicValue<Encoding, Allocator> ValueType;      //!< Value type of the document.
    typedef Encoding                        EncodingType;   //!< Character encoding type.
    typedef Allocator                       AllocatorType;  //!< Allocator type from template parameter.

public:
    BasicDocument(const AllocatorType *allocator = NULL) : mAllocator(allocator) {
        //jimi_assert(allocator != NULL);
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
    void destroy();

    BasicDocument & parse(const CharType * text);

    void visit();

private:
    const AllocatorType * mAllocator;
};

// Define default Document class type
typedef BasicDocument<>     Document;

template <typename Encoding, typename Allocator, typename StackAllocator>
void BasicDocument<Encoding, Allocator, StackAllocator>::visit()
{
    printf("JsonFx::BasicDocument::visit() visited.\n\n");
}

template <typename Encoding, typename Allocator, typename StackAllocator>
void BasicDocument<Encoding, Allocator, StackAllocator>::destroy()
{
    if (AllocatorType::kNeedFree) {
        if (mAllocator) {
            delete mAllocator;
            mAllocator = NULL;
        }
    }
}

template <typename Encoding, typename Allocator, typename StackAllocator>
BasicDocument<Encoding, Allocator, StackAllocator> &
BasicDocument<Encoding, Allocator, StackAllocator>::parse(const CharType * text)
{
    jimi_assert(text != NULL);
    printf("JsonFx::BasicDocument::parse() visited.\n\n");

    setObject();
    return *this;
}

}  // namespace JsonFx

#endif  /* !_JSONFX_DOCUMENT_H_ */
