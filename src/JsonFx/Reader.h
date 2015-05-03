
#ifndef _JSONFX_READER_H_
#define _JSONFX_READER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Allocator.h"
#include "JsonFx/Stack.h"
#include "JsonFx/ParseResult.h"

namespace JsonFx {

// Forward declaration.
template <typename SourceEncodingT,
          typename TargetEncodingT,
          typename StackAllocatorT = DefaultPoolAllocator>
class BasicReader;

// Define default Reader class type
typedef BasicReader<DefaultEncoding, DefaultEncoding>   Reader;

template <typename SourceEncodingT,
          typename TargetEncodingT,
          typename StackAllocatorT /* = DefaultPoolAllocator */>
class BasicReader {
public:
    typedef typename SourceEncodingT::CharType  CharType;   //!< SourceEncoding character type

    static const size_t kDefaultStackCapacity = 256;

private:
    BasicStack<StackAllocatorT>         mStack;
    BasicParseResult<SourceEncodingT>   mParseResult;

public:
    BasicReader(StackAllocatorT * stackAllocator = NULL, size_t stackCapacity = kDefaultStackCapacity)
        : mStack(stackAllocator, stackCapacity), mParseResult()
    {
        /* Do nothing! */
    }

    ~BasicReader() {}

    template <unsigned parseFlags, typename InputStreamT, typename HandlerT>
    ParseResult & parse() {
        return mParseResult;
    }
};

}  // namespace JsonFx

// Define default Reader class type
typedef JsonFx::BasicReader<JsonFx::DefaultEncoding, JsonFx::DefaultEncoding>   jfxReader;

#endif  /* !_JSONFX_READER_H_ */
