
#ifndef _JSONFX_ALLOCATOR_H_
#define _JSONFX_ALLOCATOR_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <memory.h>
#include <memory>

#include "JsonFx/Config.h"

/* Whether use profile codes when use JsonFx allocators? */
#define JSONFX_ALLOCATOR_USE_PROFILE    1

/* If it is 0, don't allow malloc size more than the default chunk size. */
#define JSONFX_ALLOW_ALLOC_BIGSIZE      0

/* Default malloc alignment size. */
#define JSONFX_POOL_ALIGNMENT_SIZE      8

namespace JsonFx {

/* Just define for inheritance. */
class HeapAllocator {
#if 0
public:
    static const bool kNeedFree = false;
#endif
public:
    HeapAllocator()  {}
    ~HeapAllocator() {}
};

/* Just define for inheritance. */
class PoolAllocator {
#if 0
public:
    static const bool   kNeedFree           = false;
    static const bool   kAutoRelease        = false;
    static const size_t kChunkCapacity      = 0;
    static const size_t kInnerChunkCapacity = 0;
    static const size_t kAlignmentSize      = JSONFX_POOL_ALIGNMENT_SIZE;
#endif
public:
    PoolAllocator()  {}
    ~PoolAllocator() {}
};

// C-RunTime Allocator
class TrivialAllocator : public HeapAllocator
{
public:
    static const bool kNeedFree = true;

    TrivialAllocator()  {}
    ~TrivialAllocator() {}

    static void * malloc(size_t size)  { return std::malloc(size); }
    static void * realloc(void * ptr, size_t size, size_t new_size) {
        (void)size;
        return std::realloc(ptr, new_size);
    }
    static void free(void * ptr)              { std::free(ptr); }
    static void free(void * ptr, size_t size) { (void)size; std::free(ptr); }

    static void * aligned_malloc(size_t size, size_t alignment) {
        return ::_aligned_malloc(size, alignment);
    }
    static void aligned_free(void * ptr) {
        return ::_aligned_free(ptr);
    }
};

template <size_t ChunkCapacity = gDefaultChunkCapacity,
          size_t InnerChunkCapacity = gDefaultInnerChunkCapacity,
          typename Allocator = DefaultAllocator>
class HeapPoolAllocator : public PoolAllocator
{
public:
    typedef Allocator AllocatorType;

    static const bool   kNeedFree           = false;
    static const bool   kAutoRelease        = false;
    static const size_t kChunkCapacity      = ChunkCapacity;
    static const size_t kInnerChunkCapacity = InnerChunkCapacity;
    static const size_t kAlignmentSize      = JSONFX_POOL_ALIGNMENT_SIZE;

    HeapPoolAllocator()  {}
    ~HeapPoolAllocator() {}

    void destroy() { /* Do nothing! */ }

    void * allocate(size_t size) { return AllocatorType::malloc(size); }

    void * allocateLarge(size_t size) { return AllocatorType::malloc(size); }

    void * reallocate(void * ptr, size_t size, size_t new_size) {
        return AllocatorType::realloc(ptr, size, new_size);
    }

    static void deallocate(void * ptr) { return AllocatorType::free(ptr); }
    static void deallocate(void * ptr, size_t size) { return AllocatorType::free(ptr, size); }
};

}  // namespace JsonFx

#include "JsonFx/Allocator/StdPoolAllocator.h"
#include "JsonFx/Allocator/SimplePoolAllocator.h"
#include "JsonFx/Allocator/FastPoolAllocator.h"

#endif  /* !_JSONFX_ALLOCATOR_H_ */
