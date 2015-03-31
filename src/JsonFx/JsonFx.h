
#ifndef _JSONFX_H_
#define _JSONFX_H_

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

}

#endif  /* !_JSONFX_H_ */
