
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
    void visit();

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
        if (size <= mChunkHeader->remain) {
            void * buffer = reinterpret_cast<void *>(mChunkHeader->cursor);
            jimi_assert(buffer != NULL);

            JIMI_PVOID_ADD(mChunkHeader->cursor, size);
            mChunkHeader->remain -= size;
            return buffer;
        }
        else {
            // Add a default cappacity size chunk
            internal_AddNewChunk();

            jimi_assert(mChunkHeader != NULL);
            jimi_assert(mChunkHeader->cursor != NULL);

            return mChunkHeader->cursor;
        }
        return NULL;
    }

    void * realloc(void *ptr, size_t size, size_t new_size) {
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

template <size_t ChunkCapacity, typename BaseAllocator>
void MemoryPoolAllocator<ChunkCapacity, BaseAllocator>::visit()
{
    printf("JsonFx::MemoryPoolAllocator::visit() visited.\n\n");
}

}  // namespace JsonFx

#endif  /* !_JSONFX_ALLOCATOR_H_ */
