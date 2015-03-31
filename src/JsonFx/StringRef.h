
#ifndef _JSONFX_STRINGREF_H_
#define _JSONFX_STRINGREF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Config.h"

namespace JsonFx {

template <typename CharType = JSONFX_DEFAULT_CHARTYPE>
class BaseStringRef {
public:
    typedef CharType    char_type;

public:
    BaseStringRef()  {}
    ~BaseStringRef() {}

    void visit();
};

// Define default StringRef class type
typedef BaseStringRef<>     StringRef;

template <typename CharType>
void BaseStringRef<CharType>::visit()
{
    printf("JsonFx::BaseStringRef::visit() visited.\n\n");
}

}  // namespace JsonFx

#endif  /* !_JSONFX_STRINGREF_H_ */
