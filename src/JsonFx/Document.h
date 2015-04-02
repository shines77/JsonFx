
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

template <typename Encoding = JSONFX_DEFAULT_ENCODING, typename Allocator = DefaultPoolAllocator>
class BasicDocument : public BasicValue<Encoding, Allocator>
{
public:
    typedef typename Encoding::CharType     CharType;
    typedef typename Encoding::CharType     char_type;
    typedef Encoding                        EncodingType;

public:
    BasicDocument()  {}
    ~BasicDocument() {}

    BasicDocument & parse(const char_type * text);

    void visit();
};

// Define default Document class type
typedef BasicDocument<>     Document;

template <typename Encoding, typename Allocator>
void BasicDocument<Encoding, Allocator>::visit()
{
    printf("JsonFx::Document::visit() visited.\n\n");
}

template <typename Encoding, typename Allocator>
BasicDocument<Encoding, Allocator> &
BasicDocument<Encoding, Allocator>::parse(const char_type * text)
{
    jimi_assert(text != NULL);
    printf("JsonFx::Document::parse() visited.\n\n");

    setObject();
    return *this;
}

}  // namespace JsonFx

#endif  /* !_JSONFX_DOCUMENT_H_ */
