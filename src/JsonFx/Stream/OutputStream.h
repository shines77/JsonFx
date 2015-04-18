
#ifndef _JSONFX_OUTPUTSTREAM_H_
#define _JSONFX_OUTPUTSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Internal/Closeable.h"

namespace JsonFx {

class OutputStream : public internal::Closeable
{
public:
    OutputStream()  {}
    ~OutputStream() {}

    void close();
};

}  // namespace JsonFx

#endif  /* _JSONFX_OUTPUTSTREAM_H_ */
