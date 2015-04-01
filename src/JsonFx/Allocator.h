
#ifndef _JSONFX_ALLOCATOR_H_
#define _JSONFX_ALLOCATOR_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>
#include <stdlib.h>

#include "JsonFx/Config.h"
#include "JsonFx/CharSet.h"
#include "JsonFx/Value.h"

#include "jimi/basic/assert.h"

namespace JsonFx {

class CrtAllocator
{
public:
    static const bool kNeedFree = true;

    void * malloc(size_t size)  { return std::malloc(size); }
    void * realloc(void *ptr, size_t size, size_t new_size) {
        (void)size;
        return std::realloc(ptr, new_size);
    }
    static void free(void *ptr) { std::free(ptr); }
};

template <typename BaseAllocator = CrtAllocator>
class MemoryPoolAllocator
{
public:
    static const bool kNeedFree = false;

public:
    MemoryPoolAllocator()  {}
    ~MemoryPoolAllocator() {}

    void visit();

    void * malloc(size_t size)  { return std::malloc(size); }
    void * realloc(void *ptr, size_t size, size_t new_size) {
        (void)size;
        return std::realloc(ptr, new_size);
    }
    static void free(void *ptr) { std::free(ptr); }    
};

template <typename BaseAllocator>
void MemoryPoolAllocator<BaseAllocator>::visit()
{
    printf("JsonFx::MemoryPoolAllocator::visit() visited.\n\n");
}

}  // namespace JsonFx

#endif  /* !_JSONFX_ALLOCATOR_H_ */
