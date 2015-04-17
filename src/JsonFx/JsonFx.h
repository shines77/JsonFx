
#ifndef _JSONFX_H_
#define _JSONFX_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Config.h"
#include "JsonFx/Allocator.h"
#include "JsonFx/Value.h"
#include "JsonFx/Document.h"

namespace JsonFx {

class Json {
public:
    Json()  {}
    ~Json() {}

    void visit();
};

}  // namespace JsonFx

#endif  /* !_JSONFX_H_ */
