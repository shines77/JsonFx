
#ifndef _JSONFX_VALUE_H_
#define _JSONFX_VALUE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

namespace JsonFx {

class Value {
public:
    Value()  {}
    ~Value() {}

    void get();
};

void Value::get()
{
    printf("JsonFx::Value::get() visited.\n\n");
}

}  // namespace JsonFx

#endif  /* !_JSONFX_VALUE_H_ */
