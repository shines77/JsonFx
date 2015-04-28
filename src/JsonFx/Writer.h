
#ifndef _JSONFX_WRITER_H_
#define _JSONFX_WRITER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Allocator.h"

namespace JsonFx {

// Forward declaration.
template <typename SourceEncodingT,
          typename TargetEncodingT,
          typename StackAllocatorT = TrivialAllocator>
class BasicWriter;

// Define default Writer class type
typedef BasicWriter<DefaultEncoding, DefaultEncoding>   Writer;

template <typename SourceEncodingT,
          typename TargetEncodingT,
          typename StackAllocatorT /* = TrivialAllocator */>
class BasicWriter {
public:
    BasicWriter()  {}
    ~BasicWriter() {}

    void visit();
};

}  // namespace JsonFx

// Define default Writer class type
typedef JsonFx::BasicWriter<JsonFx::DefaultEncoding, JsonFx::DefaultEncoding>   jfxWriter;

#endif  /* !_JSONFX_WRITER_H_ */
