
#ifndef _JSONFX_STD_POOL_ALLOCATOR_H_
#define _JSONFX_STD_POOL_ALLOCATOR_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "JsonFx/Allocator.h"
#include "JsonFx/Internal/Utils.h"

#include "jimi/basic/stddef.h"
#include "jimi/basic/assert.h"

namespace JsonFx {

template <size_t ChunkCapacity = gDefaultChunkCapacity,
          size_t InnerChunkCapacity = gDefaultInnerChunkCapacity,
          typename Allocator = DefaultAllocator>
class StdPoolAllocator : public PoolAllocator
{
public:
    typedef Allocator   AllocatorType;

    static const bool   kNeedFree           = false;
    static const bool   kAutoRelease        = true;
    static const size_t kChunkCapacity      = ChunkCapacity;
    static const size_t kInnerChunkCapacity = InnerChunkCapacity;
    static const size_t kAlignmentSize      = JSONFX_POOL_ALIGNMENT_SIZE;

    struct ChunkHead {
        void *      cursor;
        size_t      remain;
        size_t      capacity;
        ChunkHead * next;
    };

private:
    // The chunk head info
    ChunkHead * mChunkHead;
    // The inner buffer on stack
    char        mInnerBuffer[kInnerChunkCapacity];

public:
    StdPoolAllocator() : mChunkHead(NULL) {
        init();
    }

    ~StdPoolAllocator() {
        destroy();
    }

    JIMI_FORCEINLINE
    void release() {
        if (this->kAutoRelease) {
            ChunkHead * pChunkHead = mChunkHead;
            while (pChunkHead != NULL) {
                ChunkHead * next = pChunkHead->next;
                if (next != NULL) {
                    AllocatorType::free(pChunkHead);
                }
                pChunkHead = next;
            }
        }
    }

private:
    //! Copy constructor is not permitted.
    StdPoolAllocator(const StdPoolAllocator & rhs);               /* = delete */
    //! Copy assignment operator is not permitted.
    StdPoolAllocator & operator =(const StdPoolAllocator & rhs);  /* = delete */

    JIMI_FORCEINLINE
    void destroy() {
        if (this->kAutoRelease) {
            release();
            mChunkHead = NULL;
        }
    }

    void init() {
        if (kInnerChunkCapacity > (sizeof(ChunkHead) + kAlignmentSize))
            initInnerChunk();
        else
            addNewChunk(0);
    }

    void initInnerChunk() {
        ChunkHead * newChunk = reinterpret_cast<ChunkHead *>(mInnerBuffer);

        // Do not handle out-of-memory explicitly.
        newChunk->cursor    = reinterpret_cast<void *>(newChunk + 1);
        newChunk->remain    = kInnerChunkCapacity - sizeof(ChunkHead);
        newChunk->capacity  = kInnerChunkCapacity;
        newChunk->next      = mChunkHead;

        mChunkHead = newChunk;
    }

    void * addNewChunk(size_t size) {
        ChunkHead * newChunk = reinterpret_cast<ChunkHead *>(AllocatorType::malloc(kChunkCapacity));
        jimi_assert(newChunk != NULL);

        void * cursor = reinterpret_cast<void *>(newChunk + 1);

        // Do not handle out-of-memory explicitly.
        newChunk->cursor    = reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + size);
        newChunk->remain    = kChunkCapacity - (sizeof(ChunkHead) + size);
        newChunk->capacity  = kChunkCapacity;
        newChunk->next      = mChunkHead;

        mChunkHead = newChunk;
        return cursor;
    }

    void * addNewChunk(size_t nChunkCapacity, size_t size) {
        ChunkHead * newChunk = reinterpret_cast<ChunkHead *>(AllocatorType::malloc(nChunkCapacity));
        jimi_assert(newChunk != NULL);

        void * cursor = reinterpret_cast<void *>(newChunk + 1);

        // Do not handle out-of-memory explicitly.
        newChunk->cursor    = reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + size);
        newChunk->remain    = nChunkCapacity - (sizeof(ChunkHead) + size);
        newChunk->capacity  = nChunkCapacity;
        newChunk->next      = mChunkHead;

        mChunkHead = newChunk;
        return cursor;
    }

public:
    void * allocate(size_t size) {
        void * buffer;
        // Default alignment size is 8
        size = JIMI_ALIGNED_TO(size, kAlignmentSize);
        // If the chunk has enough space to allocate size bytes
        if (size <= mChunkHead->remain) {
            buffer = reinterpret_cast<char *>(mChunkHead->cursor);
            jimi_assert(buffer != NULL);

            //JIMI_PVOID_ADD(mChunkHeader->cursor, size);
            mChunkHead->cursor = reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead->cursor) + size);

            mChunkHead->remain -= size;
            jimi_assert((ssize_t)mChunkHead->remain >= 0);
            return buffer;
        }
        else {
#if !defined(JSONFX_ALLOW_ALLOC_BIGSIZE) || (JSONFX_ALLOW_ALLOC_BIGSIZE == 0)
            // Add a default capacity size chunk
            buffer = addNewChunk(size);
#else  /* !defined(JSONFX_ALLOW_ALLOC_BIGSIZE) */
            // Add a default capacity size chunk
            if (size <= kChunkCapacity) {
                buffer = addNewChunk(size);
            }
            else {
                size_t newAllocSize = size + sizeof(ChunkInfo);
                size_t newChunkCapacity = internal::RoundToPowerOf2(newAllocSize);
                buffer = addNewChunk(newChunkCapacity, size);
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
        if (ptr == reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead + 1)
                + (mChunkHead->capacity - mChunkHead->remain - sizeof(ChunkInfo)) - size)) {
            size_t increment = static_cast<size_t>(new_size - size);
            increment = JIMI_ALIGNED_TO(increment, kAlignmentSize);
            if (increment <= mChunkHead->remain) {
                mChunkHead->remain -= increment;
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
};

}  // namespace JsonFx

#endif  /* _JSONFX_STD_POOL_ALLOCATOR_H_ */
