
#ifndef _JSONFX_DOCUMENT_H_
#define _JSONFX_DOCUMENT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Config.h"
#include "JsonFx/CharSet.h"
#include "JsonFx/Value.h"

#include "jimi/basic/assert.h"

namespace JsonFx {

template <typename Encoding = JSONFX_DEFAULT_ENCODING, typename Allocator = DefaultAllocator>
class BaseDocument : public BaseValue<Encoding, Allocator>
{
public:
    typedef typename Encoding::CharType     CharType;
    typedef typename Encoding::CharType     char_type;
    typedef Encoding                        EncodingType;

public:
    BaseDocument()  {}
    ~BaseDocument() {}

    BaseDocument & parse(const char_type * text);

    void visit();
};

// Define default Document class type
typedef BaseDocument<>     Document;

template <typename Encoding, typename Allocator>
void BaseDocument<Encoding, Allocator>::visit()
{
    printf("JsonFx::Document::visit() visited.\n\n");
}

template <typename Encoding, typename Allocator>
BaseDocument<Encoding, Allocator> &
BaseDocument<Encoding, Allocator>::parse(const char_type * text)
{
    jimi_assert(text != NULL);
    printf("JsonFx::Document::parse() visited.\n\n");
    return *this;
}


}  // namespace JsonFx

#endif  /* !_JSONFX_DOCUMENT_H_ */
