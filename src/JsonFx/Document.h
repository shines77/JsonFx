
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

template <typename EncodingType = JSONFX_DEFAULT_ENCODING>
class BaseDocument : public BaseValue<EncodingType>
{
public:
    typedef typename EncodingType::CharType CharType;

public:
    BaseDocument()  {}
    ~BaseDocument() {}

    BaseDocument & parse(const CharType * text);

    void visit();
};

// Define default Document class type
typedef BaseDocument<>     Document;

template <typename EncodingType>
void BaseDocument<EncodingType>::visit()
{
    printf("JsonFx::Document::visit() visited.\n\n");
}

template <typename EncodingType /*= CharSet::UTF8 */>
BaseDocument<EncodingType> &
BaseDocument<EncodingType>::parse(const CharType * text)
{
    jimi_assert(text != NULL);
    printf("JsonFx::Document::parse() visited.\n\n");
    return *this;
}


}  // namespace JsonFx

#endif  /* !_JSONFX_DOCUMENT_H_ */
