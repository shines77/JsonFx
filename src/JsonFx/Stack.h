
#ifndef _JSONFX_INTERNAL_STACK_H_
#define _JSONFX_INTERNAL_STACK_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Allocator.h"

namespace JsonFx {

// Forward declaration.
template <typename PoolAllocatorT = DefaultPoolAllocator>
class BasicStack;

// Define default Stack class type
typedef BasicStack<>    Stack;

template <typename PoolAllocatorT /* = DefaultPoolAllocator */>
class BasicStack {

private:
    PoolAllocatorT *    mAllocator;
    size_t              mCapacity;

public:
    BasicStack(const PoolAllocatorT * allocator, size_t capacity)
        : mAllocator(const_cast<PoolAllocatorT *>(allocator)), mCapacity(capacity)
    {
        /* Do nothing! */
    }

    ~BasicStack() {}
};

}  // namespace JsonFx

// Define default Stack class type
typedef JsonFx::BasicStack<JsonFx::DefaultPoolAllocator>    jfxStack;

#endif  /* !_JSONFX_INTERNAL_STACK_H_ */
