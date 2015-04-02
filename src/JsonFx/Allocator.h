
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

#include "jimi/basic/stddef.h"
#include "jimi/basic/assert.h"

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
#if defined(_M_X64) || defined(_WIN64) || defined(_M_AMD64) || defined(_M_IA64) || defined(_M_X86_64)
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

class CrtAllocator
{
public:
    static const bool kNeedFree = true;

    void * malloc(size_t size)  { return std::malloc(size); }
    void * realloc(void * ptr, size_t size, size_t new_size) {
        (void)size;
        return std::realloc(ptr, new_size);
    }
    static void free(void * ptr) { std::free(ptr); }
};

template <size_t ChunkCapacity = kDefaultChunkCapacity,
          typename BaseAllocator = CrtAllocator>
class MemoryPoolAllocator
{
public:
    typedef BaseAllocator   AllocatorType;

    static const bool   kNeedFree       = false;
    static const size_t kChunkCapacity  = ChunkCapacity;

    struct ChunkHeader {
        void *          cursor;
        size_t          remain;
        size_t          capacity;
        ChunkHeader *   next;
    };

public:
    MemoryPoolAllocator() : mChunkHeader(NULL) {
        internal_AddNewChunk();
    }
    ~MemoryPoolAllocator() {
        release();
    }

private:
    //! Copy constructor is not permitted.
    MemoryPoolAllocator(const MemoryPoolAllocator & rhs);               /* = delete */
    //! Copy assignment operator is not permitted.
    MemoryPoolAllocator& operator =(const MemoryPoolAllocator & rhs);   /* = delete */

    void release() {
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

    void internal_AddNewChunk() {
        ChunkHeader * newChunk = reinterpret_cast<ChunkHeader *>(AllocatorType::malloc(kChunkCapacity));
        jimi_assert(newChunk != NULL);

        // Do not handle out-of-memory explicitly.
        newChunk->cursor    = reinterpret_cast<void *>(newChunk + 1);
        newChunk->remain    = kChunkCapacity;
        newChunk->capacity  = kChunkCapacity;
        newChunk->next      = mChunkHeader;

        mChunkHeader = newChunk;
    }

    void internal_AddNewChunk(size_t nChunkCapacity) {
        ChunkHeader * newChunk = reinterpret_cast<ChunkHeader *>(AllocatorType::malloc(nChunkCapacity));
        jimi_assert(newChunk != NULL);

        // Do not handle out-of-memory explicitly.
        newChunk->cursor    = reinterpret_cast<void *>(newChunk + 1);
        newChunk->remain    = nChunkCapacity;
        newChunk->capacity  = nChunkCapacity;
        newChunk->next      = mChunkHeader;

        mChunkHeader = newChunk;
    }

public:
    ChunkHeader * addNewChunk() {
        ChunkHeader * newChunk = reinterpret_cast<ChunkHeader *>(AllocatorType::malloc(kChunkCapacity));
        jimi_assert(newChunk != NULL);

        if (newChunk != NULL) {
            // Do not handle out-of-memory explicitly.
            newChunk->cursor    = reinterpret_cast<void *>(newChunk + 1);
            newChunk->remain    = kChunkCapacity;
            newChunk->capacity  = kChunkCapacity;
            newChunk->next      = mChunkHeader;

            mChunkHeader = newChunk;
            return newChunk;
        }
        return NULL;
    }

    ChunkHeader * addNewChunk(size_t nChunkCapacity) {
        ChunkHeader * newChunk = reinterpret_cast<ChunkHeader *>(AllocatorType::malloc(nChunkCapacity));
        jimi_assert(newChunk != NULL);

        if (newChunk != NULL) {
            newChunk->cursor    = reinterpret_cast<void *>(newChunk + 1);
            newChunk->remain    = nChunkCapacity;
            newChunk->capacity  = nChunkCapacity;
            newChunk->next      = mChunkHeader;

            mChunkHeader = newChunk;
            return newChunk;
        }
        return NULL;
    }

    void * malloc(size_t size) {
        size = JIMI_ALIGNED_TO(size, 8);
        if (size > mChunkHeader->remain) {
            // Add a default cappacity size chunk
            if (size <= kDefaultChunkCapacity) {
                internal_AddNewChunk();
            }
            else {
                size_t newChunkCapacity = internal::RoundToPowerOf2(size);
                internal_AddNewChunk(newChunkCapacity);
            }
            jimi_assert(mChunkHeader != NULL);
            jimi_assert(mChunkHeader->cursor != NULL);
        }
        void * buffer = reinterpret_cast<void *>(mChunkHeader->cursor);
        jimi_assert(buffer != NULL);

        JIMI_PVOID_ADD(mChunkHeader->cursor, size);
        mChunkHeader->remain -= size;
        return buffer;
    }

    void * realloc(const void *ptr, size_t size, size_t new_size) {
        // Do not shrink if new size is smaller than original
        if (size >= new_size)
            return ptr;

        // Simply expand it if it is the last allocation and there is sufficient space
        if (ptr == reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHeader + 1)
                + (mChunkHeader->capacity - mChunkHeader->remain) - size)) {
            size_t increment = static_cast<size_t>(new_size - size);
            increment = JIMI_ALIGNED_TO(increment, 8);
            if (increment <= mChunkHeader->remain) {
                mChunkHeader->remain -= increment;
                return ptr;
            }
        }

        // Realloc process: allocate and copy memory, do not free original buffer.
        void * newBuffer = this->malloc(new_size);
        jimi_assert(newBuffer != NULL);     // Do not handle out-of-memory explicitly.
        return std::memcpy(newBuffer, ptr, size);
    }

    static void free(void * ptr) { (void)ptr; }      /* Do nothing! */

private:
    ChunkHeader * mChunkHeader;
};

template <size_t ChunkCapacity = kDefaultChunkCapacity,
          typename BaseAllocator = CrtAllocator>
class SimpleMemoryPoolAllocator
{
public:
    typedef BaseAllocator   AllocatorType;

    static const bool   kNeedFree       = false;
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
        size_t      sizeTotal;
        size_t      capacityTotal;
    };

public:
    SimpleMemoryPoolAllocator() : mChunkHead() {
        initChunkHead();
        internal_AddNewChunk();
    }
    ~SimpleMemoryPoolAllocator() {
        release();
    }

private:
    //! Copy constructor is not permitted.
    SimpleMemoryPoolAllocator(const SimpleMemoryPoolAllocator & rhs);               /* = delete */
    //! Copy assignment operator is not permitted.
    SimpleMemoryPoolAllocator& operator =(const SimpleMemoryPoolAllocator & rhs);   /* = delete */

    void release() {
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

    void initChunkHead() {
        mChunkHead.cursor = NULL;
        mChunkHead.remain = 0;
        mChunkHead.capacity = 0;
        mChunkHead.next = NULL;
        mChunkHead.sizeTotal = 0;
        mChunkHead.capacityTotal = 0;
    }

    void internal_AddNewChunk() {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(AllocatorType::malloc(kChunkCapacity));
        jimi_assert(newChunk != NULL);

        // Do not handle out-of-memory explicitly.
        newChunk->next              = mChunkHead.next;
        newChunk->capacity          = kChunkCapacity;

        mChunkHead.cursor           = reinterpret_cast<void *>(newChunk + 1);
        mChunkHead.remain           = kChunkCapacity;
        mChunkHead.capacity         = kChunkCapacity;
        mChunkHead.capacityTotal   += kChunkCapacity;

        mChunkHead.next = newChunk;
    }

    void internal_AddNewChunk(size_t nChunkCapacity) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(AllocatorType::malloc(nChunkCapacity));
        jimi_assert(newChunk != NULL);

        // Do not handle out-of-memory explicitly.
        newChunk->next              = mChunkHead.next;
        newChunk->capacity          = nChunkCapacity;

        mChunkHead.cursor           = reinterpret_cast<void *>(newChunk + 1);
        mChunkHead.remain           = nChunkCapacity;
        mChunkHead.capacity         = nChunkCapacity;
        mChunkHead.capacityTotal   += nChunkCapacity;

        mChunkHead.next = newChunk;
    }

public:
    ChunkInfo * addNewChunk() {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(AllocatorType::malloc(kChunkCapacity));
        jimi_assert(newChunk != NULL);

        if (newChunk != NULL) {
            newChunk->next              = mChunkHead.next;
            newChunk->capacity          = kChunkCapacity;

            mChunkHead.cursor           = reinterpret_cast<void *>(newChunk + 1);
            mChunkHead.remain           = kChunkCapacity;
            mChunkHead.capacity         = kChunkCapacity;
            mChunkHead.capacityTotal   += kChunkCapacity;

            mChunkHead.next = newChunk;
            return newChunk;
        }
        return NULL;
    }

    ChunkInfo * addNewChunk(size_t nChunkCapacity) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(AllocatorType::malloc(nChunkCapacity));
        jimi_assert(newChunk != NULL);

        if (newChunk != NULL) {
            newChunk->next              = mChunkHead.next;
            newChunk->capacity          = nChunkCapacity;

            mChunkHead.cursor           = reinterpret_cast<void *>(newChunk + 1);
            mChunkHead.remain           = nChunkCapacity;
            mChunkHead.capacity         = nChunkCapacity;
            mChunkHead.capacityTotal   += nChunkCapacity;

            mChunkHead.next = newChunk;
            return newChunk;
        }
        return NULL;
    }

    void * malloc(size_t size) {
        size = JIMI_ALIGNED_TO(size, 8);
        if (size > mChunkHead.remain) {
            // Calc the real allocate size of the actived chunk
            size_t realAllocateSize = mChunkHead.capacity - mChunkHead.remain;
            mChunkHead.sizeTotal += realAllocateSize;

            // Add a default capacity size chunk
            if (size <= kDefaultChunkCapacity) {
                internal_AddNewChunk();
            }
            else {
                size_t newChunkCapacity = internal::RoundToPowerOf2(size);
                internal_AddNewChunk(newChunkCapacity);
            }

            jimi_assert(mChunkHead.next != NULL);
            jimi_assert(mChunkHead.cursor != NULL);
        }

        void * buffer = reinterpret_cast<void *>(mChunkHead.cursor);
        jimi_assert(buffer != NULL);

        JIMI_PVOID_ADD(mChunkHead.cursor, size);
        mChunkHead.remain -= size;
        return buffer;
    }

    void * realloc(const void *ptr, size_t size, size_t new_size) {
        // Do not shrink if new size is smaller than original
        if (size >= new_size)
            return ptr;

        // Simply expand it if it is the last allocation and there is sufficient space
        if (ptr == reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead.next + 1)
                + (mChunkHead.capacity - mChunkHead.remain) - size)) {
            size_t increment = static_cast<size_t>(new_size - size);
            increment = JIMI_ALIGNED_TO(increment, 8);
            if (increment <= mChunkHead.remain) {
                mChunkHead.remain -= increment;
                return ptr;
            }
        }

        // Realloc process: allocate and copy memory, do not free original buffer.
        void * newBuffer = this->malloc(new_size);
        jimi_assert(newBuffer != NULL);     // Do not handle out-of-memory explicitly.
        return std::memcpy(newBuffer, ptr, size);
    }

    static void free(void * ptr) { (void)ptr; }      /* Do nothing! */

private:
    ChunkHead   mChunkHead;
};

}  // namespace JsonFx

#endif  /* !_JSONFX_ALLOCATOR_H_ */
