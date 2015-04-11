
#ifndef _JSONFX_ALLOCATOR_H_
#define _JSONFX_ALLOCATOR_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>
#include <stdlib.h>

#include <memory>

#include "JsonFx/Config.h"
#include "JsonFx/CharSet.h"
#include "JsonFx/Value.h"

#include "jimi/basic/arch_def.h"
#include "jimi/basic/stddef.h"
#include "jimi/basic/assert.h"

/* Whether use profile codes when use JsonFx allocators? */
#define JSONFX_ALLOCATOR_USE_PROFILE    1

/* If it is 0, don't allow malloc size more than the default chunk size. */
#define JSONFX_ALLOW_ALLOC_BIGSIZE      0

/* Default malloc alignment size. */
#define JSONFX_MALLOC_ALIGNMENT_SIZE    8

namespace JsonFx {

namespace internal {

size_t RoundToPowerOf2(size_t n) {
#if 1
    if (n == 0)
        return 0;

    // ms1b
    --n;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
#if defined(JIMI_ARCH_X64) || defined(JIMI_ARCH_IA64)
    n |= n >> 32;
#endif
    return ++n;

#else
    size_t ms1b = 1;
    while (ms1b < n) {
        ms1b <<= 1;
    }

    return ms1b;
#endif
}

}  // namespace internal

/* Just define for inheritance. */
class StackAllocator {};

/* Just define for inheritance. */
class PoolAllocator {};

class CrtAllocator : public StackAllocator
{
public:
    static const bool kNeedFree = true;

    static void * malloc(size_t size)  { return std::malloc(size); }
    static void * realloc(void * ptr, size_t size, size_t new_size) {
        (void)size;
        return std::realloc(ptr, new_size);
    }
    static void free(void * ptr) { std::free(ptr); }
    static void free(void * ptr, size_t size) { (void)size; std::free(ptr); }
};

template <size_t ChunkCapacity = kDefaultChunkCapacity,
          typename Allocator = DefaultAllocator>
class StackPoolAllocator : public PoolAllocator
{
public:
    typedef Allocator AllocatorType;

    static const bool kNeedFree = false;

    StackPoolAllocator()  {}
    ~StackPoolAllocator() {}

    void destroy() { /* Do nothing! */ }

    void * allocate(size_t size) { return AllocatorType::malloc(size); }

    void * reallocate(void * ptr, size_t size, size_t new_size) {
        return AllocatorType::realloc(ptr, size, new_size);
    }

    static void deallocate(void * ptr) { return AllocatorType::free(ptr); }
    static void deallocate(void * ptr, size_t size) { return AllocatorType::free(ptr, size); }
};

template <size_t ChunkCapacity = kDefaultChunkCapacity,
          typename Allocator = DefaultAllocator>
class MemoryPoolAllocator : public PoolAllocator
{
public:
    typedef Allocator   AllocatorType;

    static const bool   kNeedFree       = true;
    static const size_t kChunkCapacity  = ChunkCapacity;

    struct ChunkHeader {
        void *          cursor;
        size_t          remain;
        size_t          capacity;
        ChunkHeader *   next;
    };

public:
    MemoryPoolAllocator() : mChunkHeader(NULL) {
        addNewChunk(0);
    }
    ~MemoryPoolAllocator() {
        destroy();
    }

    void destroy() {
        if (AllocatorType::kNeedFree) {
            ChunkHeader * pChunkHeader = mChunkHeader;
            while (pChunkHeader != NULL) {
                ChunkHeader * next = pChunkHeader->next;
                AllocatorType::free(pChunkHeader);
                pChunkHeader = next;
            }
            mChunkHeader = NULL;
        }
    }

private:
    //! Copy constructor is not permitted.
    MemoryPoolAllocator(const MemoryPoolAllocator & rhs);               /* = delete */
    //! Copy assignment operator is not permitted.
    MemoryPoolAllocator& operator =(const MemoryPoolAllocator & rhs);   /* = delete */

    void * addNewChunk(size_t size) {
        ChunkHeader * newChunk = reinterpret_cast<ChunkHeader *>(AllocatorType::malloc(kChunkCapacity));
        jimi_assert(newChunk != NULL);

        void * cursor = reinterpret_cast<void *>(newChunk + 1);

        // Do not handle out-of-memory explicitly.
        newChunk->cursor    = reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + size);
        newChunk->remain    = kChunkCapacity - sizeof(ChunkHeader) - size;
        newChunk->capacity  = kChunkCapacity;
        newChunk->next      = mChunkHeader;

        mChunkHeader = newChunk;
        return cursor;
    }

    void * addNewChunk(size_t nChunkCapacity, size_t size) {
        ChunkHeader * newChunk = reinterpret_cast<ChunkHeader *>(AllocatorType::malloc(nChunkCapacity));
        jimi_assert(newChunk != NULL);

        void * cursor = reinterpret_cast<void *>(newChunk + 1);

        // Do not handle out-of-memory explicitly.
        newChunk->cursor    = reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + size);
        newChunk->remain    = nChunkCapacity - sizeof(ChunkHeader) - size;
        newChunk->capacity  = nChunkCapacity;
        newChunk->next      = mChunkHeader;

        mChunkHeader = newChunk;
        return cursor;
    }

public:
    void * allocate(size_t size) {
        void * buffer;
        // Default alignment size is 8
        size = JIMI_ALIGNED_TO(size, JSONFX_MALLOC_ALIGNMENT_SIZE);
        if (size <= mChunkHeader->remain) {
            buffer = reinterpret_cast<void *>(mChunkHeader->cursor);
            jimi_assert(buffer != NULL);

            // mChunkHeader->cursor += size;
            //JIMI_PVOID_ADD(mChunkHeader->cursor, size);
            mChunkHeader->cursor = reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHeader->cursor) + size);

            mChunkHeader->remain -= size;
            jimi_assert((ssize_t)mChunkHeader->remain >= 0);
            return buffer;
        }
        else {
#if !defined(JSONFX_ALLOW_ALLOC_BIGSIZE) || (JSONFX_ALLOW_ALLOC_BIGSIZE == 0)
            // Add a default capacity size chunk
            buffer = addNewChunk();
#else  /* !defined(JSONFX_ALLOW_ALLOC_BIGSIZE) */
            // Add a default capacity size chunk
            if (size <= kDefaultChunkCapacity) {
                buffer = addNewChunk();
            }
            else {
                size_t newAllocSize = size + sizeof(ChunkInfo);
                size_t newChunkCapacity = internal::RoundToPowerOf2(newAllocSize);
                buffer = addNewChunk(newChunkCapacity);
            }
#endif  /* defined(JSONFX_ALLOW_ALLOC_BIGSIZE) */

            jimi_assert(buffer != NULL);
            return buffer;
        }
    }

    void * reallocate(const void * ptr, size_t size, size_t new_size) {
        // Do not shrink if new size is smaller than original
        if (size >= new_size)
            return ptr;

        // Simply expand it if it is the last allocation and there is sufficient space
        if (ptr == reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHeader + 1)
                + (mChunkHeader->capacity - mChunkHeader->remain - sizeof(ChunkInfo)) - size)) {
            size_t increment = static_cast<size_t>(new_size - size);
            increment = JIMI_ALIGNED_TO(increment, JSONFX_MALLOC_ALIGNMENT_SIZE);
            if (increment <= mChunkHeader->remain) {
                mChunkHeader->remain -= increment;
                return ptr;
            }
        }

        // Realloc process: allocate and copy memory, do not free original buffer.
        void * newBuffer = this->allocate(new_size);
        jimi_assert(newBuffer != NULL);     // Do not handle out-of-memory explicitly.
        return std::memcpy(newBuffer, ptr, size);
    }

    static void deallocate(void * ptr) { (void)ptr; }                           /* Do nothing! */
    static void deallocate(void * ptr, size_t size) { (void)ptr; (void)size; }  /* Do nothing! */

private:
    ChunkHeader * mChunkHeader;
};

template <size_t ChunkCapacity = kDefaultChunkCapacity,
          typename Allocator = DefaultAllocator>
class SimpleMemoryPoolAllocator : public PoolAllocator
{
public:
    typedef Allocator   AllocatorType;

    static const bool   kNeedFree       = true;
    static const size_t kChunkCapacity  = ChunkCapacity;

    struct ChunkInfo {
        ChunkInfo * next;
        size_t      capacity;
    };

    struct ChunkHead {
        void *      cursor;
        size_t      remain;
        size_t      capacity;
        ChunkInfo * next;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        size_t      sizeTotal;
        size_t      capacityTotal;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */
    };

public:
    SimpleMemoryPoolAllocator() : mChunkHead() {
        initChunkHead();
        addNewChunk(0);
    }

    ~SimpleMemoryPoolAllocator() {
        destroy();
    }

    void destroy() {
        if (AllocatorType::kNeedFree) {
            ChunkInfo * pChunkInfo = mChunkHead.next;
            while (pChunkInfo != NULL) {
                ChunkInfo * next = pChunkInfo->next;
                AllocatorType::free(pChunkInfo);
                pChunkInfo = next;
            }
            mChunkHead.next = NULL;
        }
    }

private:
    //! Copy constructor is not permitted.
    SimpleMemoryPoolAllocator(const SimpleMemoryPoolAllocator & rhs);               /* = delete */
    //! Copy assignment operator is not permitted.
    SimpleMemoryPoolAllocator& operator =(const SimpleMemoryPoolAllocator & rhs);   /* = delete */

    void initChunkHead() {
        mChunkHead.cursor           = NULL;
        mChunkHead.remain           = 0;
        mChunkHead.capacity         = 0;
        mChunkHead.next             = NULL;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mChunkHead.sizeTotal        = 0;
        mChunkHead.capacityTotal    = 0;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */
    }

    void * addNewChunk(size_t size) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(AllocatorType::malloc(kChunkCapacity));
        jimi_assert(newChunk != NULL);

        // Do not handle out-of-memory explicitly.
        newChunk->next              = mChunkHead.next;
        newChunk->capacity          = kChunkCapacity;

        void * cursor = reinterpret_cast<void *>(newChunk + 1);

        mChunkHead.cursor           = reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + size);
        mChunkHead.remain           = kChunkCapacity - sizeof(ChunkInfo) - size;
        mChunkHead.capacity         = kChunkCapacity;
#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mChunkHead.capacityTotal   += kChunkCapacity;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */

        mChunkHead.next = newChunk;
        return cursor;
    }

    void * addNewChunk(size_t nChunkCapacity, size_t size) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(AllocatorType::malloc(nChunkCapacity));
        jimi_assert(newChunk != NULL);

        // Do not handle out-of-memory explicitly.
        newChunk->next              = mChunkHead.next;
        newChunk->capacity          = nChunkCapacity;

        void * cursor = reinterpret_cast<void *>(newChunk + 1);

        mChunkHead.cursor           = reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + size);
        mChunkHead.remain           = nChunkCapacity - sizeof(ChunkInfo) - size;
        mChunkHead.capacity         = nChunkCapacity;
#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mChunkHead.capacityTotal   += nChunkCapacity;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */

        mChunkHead.next = newChunk;
        return cursor;
    }

    void * addNewChunkToLast(size_t nChunkCapacity) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(AllocatorType::malloc(nChunkCapacity));
        jimi_assert(newChunk != NULL);

        // Do not handle out-of-memory explicitly.
        newChunk->next              = mChunkHead.next->next;
        newChunk->capacity          = nChunkCapacity;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mChunkHead.sizeTotal       += nChunkCapacity - sizeof(ChunkInfo);
        mChunkHead.capacityTotal   += nChunkCapacity;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */

        mChunkHead.next->next = newChunk;

        void * cursor = reinterpret_cast<void *>(newChunk + 1);
        return cursor;
    }

public:
    void * allocate(size_t size) {
        void * buffer;
        // Default alignment size is 8
        size = JIMI_ALIGNED_TO(size, JSONFX_MALLOC_ALIGNMENT_SIZE);
        if (size <= mChunkHead.remain) {
            void * buffer = reinterpret_cast<void *>(mChunkHead.cursor);
            jimi_assert(buffer != NULL);

            // mChunkHead.cursor += size;
            //JIMI_PVOID_ADD(mChunkHead.cursor, size);
            mChunkHead.cursor = reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead.cursor) + size);

            mChunkHead.remain -= size;
            jimi_assert((ssize_t)mChunkHead.remain >= 0);
            return buffer;
        }
        else {
#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
            // Calc the real allocate size of the actived chunk
            jimi_assert(mChunkHead.capacity >= mChunkHead.remain);
            size_t realAllocateSize = mChunkHead.capacity - mChunkHead.remain - sizeof(ChunkInfo);
            mChunkHead.sizeTotal += realAllocateSize;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */

#if !defined(JSONFX_ALLOW_ALLOC_BIGSIZE) || (JSONFX_ALLOW_ALLOC_BIGSIZE == 0)
            // Add a default capacity size chunk
            buffer = addNewChunk();
#else  /* !defined(JSONFX_ALLOW_ALLOC_BIGSIZE) */
            // Add a default capacity size chunk
            if (size <= kDefaultChunkCapacity) {
                buffer = addNewChunk(size);
            }
            else {
                size_t newAllocSize = size + sizeof(ChunkInfo);
                size_t newChunkCapacity = internal::RoundToPowerOf2(newAllocSize);
                if ((newChunkCapacity - newAllocSize) > mChunkHead.remain)
                    buffer = addNewChunk(newChunkCapacity, size);
                else
                    buffer = addNewChunkToLast(newAllocSize);
            }
#endif  /* defined(JSONFX_ALLOW_ALLOC_BIGSIZE) */

            jimi_assert(mChunkHead.next != NULL);
            jimi_assert(mChunkHead.cursor != NULL);

            jimi_assert(buffer != NULL);
            return buffer;
        }
    }

    void * reallocate(const void * ptr, size_t size, size_t new_size) {
        // Do not shrink if new size is smaller than original
        if (size >= new_size)
            return ptr;

        // Simply expand it if it is the last allocation and there is sufficient space
        if (ptr == reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead.next + 1)
                + (mChunkHead.capacity - mChunkHead.remain - sizeof(ChunkInfo)) - size)) {
            size_t increment = static_cast<size_t>(new_size - size);
            increment = JIMI_ALIGNED_TO(increment, JSONFX_MALLOC_ALIGNMENT_SIZE);
            if (increment <= mChunkHead.remain) {
                mChunkHead.remain -= increment;
                return ptr;
            }
        }

        // Realloc process: allocate and copy memory, do not free original buffer.
        void * newBuffer = this->allocate(new_size);
        jimi_assert(newBuffer != NULL);     // Do not handle out-of-memory explicitly.
        return std::memcpy(newBuffer, ptr, size);
    }

    static void deallocate(void * ptr) { (void)ptr; }                           /* Do nothing! */
    static void deallocate(void * ptr, size_t size) { (void)ptr; (void)size; }  /* Do nothing! */

private:
    ChunkHead   mChunkHead;
};

}  // namespace JsonFx

#endif  /* !_JSONFX_ALLOCATOR_H_ */
