
#ifndef _JSONFX_SIMPLE_POOL_ALLOCATOR_H_
#define _JSONFX_SIMPLE_POOL_ALLOCATOR_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "JsonFx/Allocator.h"
#include "JsonFx/Internal/Utils.h"

#include "jimi/basic/stddef.h"
#include "jimi/basic/assert.h"

namespace JsonFx {

// Save and setting the packing alignment
#pragma pack(push)
#pragma pack(1)

template <size_t ChunkCapacity = gDefaultChunkCapacity,
          size_t InnerChunkCapacity = gDefaultInnerChunkCapacity,
          typename Allocator = DefaultAllocator>
class SimplePoolAllocator : public PoolAllocator
{
public:
    typedef Allocator   AllocatorType;

    // Forward declaration.
    struct ChunkInfo;

    static const bool   kNeedFree           = false;
    static const bool   kAutoRelease        = true;
    static const size_t kChunkCapacity      = ChunkCapacity;
    static const size_t kInnerChunkCapacity = InnerChunkCapacity;
    static const size_t kAlignmentSize      = JSONFX_POOL_ALIGNMENT_SIZE;
    static const size_t kChunkCapacityLimit = sizeof(ChunkInfo);
    static const size_t kMinChunkCapacityThreshold =
                    JIMI_MAX(kChunkCapacityLimit + kAlignmentSize * 2,
                             JSONFX_POOL_MIN_CHUNK_THRESHOLD);

    struct ChunkInfo {
        ChunkInfo * next;
        size_t      capacity;
    };

    struct ChunkHead {
        void *      cursor;
        size_t      remain;
        size_t      capacity;
        ChunkInfo * head;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        size_t      usedTotal;
        size_t      capacityTotal;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */
    };

private:
    // The chunk head info
    ChunkHead   mChunkHead;
    void *      mUserBuffer;
    size_t      mUserBufSize;

    // The inner buffer on stack
    ALIGN_PREFIX(JSONFX_POOL_ALIGNMENT_SIZE)
    char        mInnerBuffer[kInnerChunkCapacity];
    ALIGN_SUFFIX(JSONFX_POOL_ALIGNMENT_SIZE);

public:
    SimplePoolAllocator() : mChunkHead(), mUserBuffer(NULL), mUserBufSize(0)
    {
        jimi_assert(kChunkCapacity >= kMinChunkCapacityThreshold);
        init();
    }

    SimplePoolAllocator(void * userBuffer, size_t bufSize)
        : mChunkHead(), mUserBuffer(userBuffer), mUserBufSize(bufSize)
    {
        jimi_assert(kChunkCapacity >= kMinChunkCapacityThreshold);
        init(userBuffer, bufSize);
    }

    ~SimplePoolAllocator() {
        destroy();
    }

    JIMI_FORCEINLINE
    void release() {
        if (this->kAutoRelease) {
            ChunkInfo * pChunkInfo = mChunkHead.head;
            while (pChunkInfo != NULL) {
                ChunkInfo * next = pChunkInfo->next;
                if (next != NULL) {
                    AllocatorType::aligned_free(pChunkInfo);
                }
                pChunkInfo = next;
            }
        }
    }

    void reset() {
        // Release the chunk lists.
        release();
        // Reset used total counter
        mChunkHead.usedTotal = 0;
        // Reset the first chunk info and counter info.
        init(mUserBuffer, mUserBufSize);
    }

    void * getUserBuffer() const     { return mUserBuffer;  }
    void * getUserBufferSize() const { return mUserBufSize; }

    void * setUserBuffer(void * userBuffer, size_t bufSize) {
        void * oldUserBuffer = mUserBuffer;
        if (size >= kMinChunkCapacityThreshold) {
            mUserBuffer  = userBuffer;
            mUserBufSize = bufSize;
            return oldUserBuffer;
        }
        return NULL;
    }

private:
    //! Copy constructor is not permitted.
    SimplePoolAllocator(const SimplePoolAllocator & rhs);               /* = delete */
    //! Copy assignment operator is not permitted.
    SimplePoolAllocator & operator =(const SimplePoolAllocator & rhs);  /* = delete */

    void destroy() {
        if (this->kAutoRelease) {
            // Release the chunk lists.
            release();
            mChunkHead.head = NULL;
        }
    }

    void init() {
#if 0
        mChunkHead.cursor   = NULL;
        mChunkHead.remain   = 0;
        mChunkHead.capacity = 0;
        mChunkHead.head     = NULL;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mChunkHead.usedTotal        = 0;
        mChunkHead.capacityTotal    = 0;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */
#endif
        if (kInnerChunkCapacity > kChunkCapacityLimit) {
            initInnerChunk();
            if (kInnerChunkCapacity <= kMinChunkCapacityThreshold)
                addNewChunk(0);
        }
        else {
            addNewChunk(0);
        }
    }

    void init(void * buffer, size_t size) {
        if (buffer != NULL) {
            if (size > kMinChunkCapacityThreshold) {
                initInnerChunk(buffer, size);
                return;
            }
        }
        init();
    }

    void initInnerChunk() {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(mInnerBuffer);

        newChunk->next      = NULL;
        newChunk->capacity  = kInnerChunkCapacity;

        mChunkHead.cursor   = reinterpret_cast<void *>(newChunk + 1);
        mChunkHead.remain   = kInnerChunkCapacity - sizeof(ChunkInfo);
        mChunkHead.capacity = kInnerChunkCapacity;

        mChunkHead.head = newChunk;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mChunkHead.capacityTotal = kInnerChunkCapacity;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */  
    }

    void initInnerChunk(void * buffer, size_t size) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(buffer);

        newChunk->next      = NULL;
        newChunk->capacity  = size;

        mChunkHead.cursor   = reinterpret_cast<void *>(newChunk + 1);
        mChunkHead.remain   = size - sizeof(ChunkInfo);
        mChunkHead.capacity = size;

        mChunkHead.head = newChunk;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mChunkHead.capacityTotal = size;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */  
    }

    void * addNewChunk(size_t nChunkCapacity, size_t size) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>
                    (AllocatorType::aligned_malloc(nChunkCapacity, kAlignmentSize));
        jimi_assert(newChunk != NULL);

        // Do not handle out-of-memory explicitly.
        newChunk->next      = mChunkHead.head;
        newChunk->capacity  = nChunkCapacity;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mChunkHead.usedTotal       += mChunkHead.capacity - mChunkHead.remain;
        mChunkHead.capacityTotal   += nChunkCapacity;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */

        void * cursor = reinterpret_cast<void *>(newChunk + 1);

        mChunkHead.cursor   = reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + size);
        mChunkHead.remain   = nChunkCapacity - (sizeof(ChunkInfo) + size);
        mChunkHead.capacity = nChunkCapacity;

        mChunkHead.head = newChunk;
        return cursor;
    }

    void * insertNewChunkToLast(size_t nChunkCapacity, size_t size) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>
                    (AllocatorType::aligned_malloc(nChunkCapacity, kAlignmentSize));
        jimi_assert(newChunk != NULL);

        // Do not handle out-of-memory explicitly.
        newChunk->next      = mChunkHead.head->next;
        newChunk->capacity  = nChunkCapacity;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mChunkHead.usedTotal       += sizeof(ChunkInfo) + size;
        mChunkHead.capacityTotal   += nChunkCapacity;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */

        mChunkHead.head->next = newChunk;

        void * cursor = reinterpret_cast<void *>(newChunk + 1);
        return cursor;
    }

public:
    size_t getUsed() const {
#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        return (mChunkHead.usedTotal + mChunkHead.capacity - mChunkHead.remain);
#else
        return 0;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */
    }

    size_t getCapacity() const {
#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        return mChunkHead.capacityTotal;
#else
        return 0;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */
    }

    void * addNewChunk(size_t size) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>
                    (AllocatorType::aligned_malloc(kChunkCapacity, kAlignmentSize));
        jimi_assert(newChunk != NULL);

        // Do not handle out-of-memory explicitly.
        newChunk->next      = mChunkHead.head;
        newChunk->capacity  = kChunkCapacity;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mChunkHead.usedTotal       += mChunkHead.capacity - mChunkHead.remain;
        mChunkHead.capacityTotal   += kChunkCapacity;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */

        void * cursor = reinterpret_cast<void *>(newChunk + 1);

        mChunkHead.cursor   = reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + size);
        mChunkHead.remain   = kChunkCapacity - (sizeof(ChunkInfo) + size);
        mChunkHead.capacity = kChunkCapacity;

        mChunkHead.head = newChunk;
        return cursor;
    }

    void * skip(size_t skipSize) {
        jimi_assert(mChunkHead.head != NULL);
        if (skipSize <= mChunkHead.remain) {
            return reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead.cursor) + skipSize);
        }
        else {
            void * cursor = addNewChunk(0);
            return reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + skipSize);
        }
    }

    void * skip(size_t skipSize, size_t reserveSize) {
        jimi_assert(mChunkHead.head != NULL);
        if ((skipSize + reserveSize) <= mChunkHead.remain) {
            return reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead.cursor) + skipSize);
        }
        else {
            void * cursor = addNewChunk(0);
            return reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + skipSize);
        }
    }

    inline void * addNewChunkAndSkip(size_t skipSize) {
        return addNewChunkAndSkip(skipSize, 0);
    }

    inline void * addNewChunkAndSkip(size_t skipSize, size_t reserveSize) {
        jimi_assert(mChunkHead.head != NULL);
        void * cursor;
        if ((skipSize + reserveSize) <= kChunkCapacity) {
            cursor = addNewChunk(0);
            return reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + skipSize);
        }
        else {
            cursor = addNewChunk(skipSize + reserveSize, 0);
            return reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + skipSize);
        }
    }

    void * getChunkCursor() const {
        jimi_assert(mChunkHead.head != NULL);
        jimi_assert(mChunkHead.cursor != NULL);
        return reinterpret_cast<void *>(mChunkHead.cursor);
    }

    void * getChunkBottom() const {
        jimi_assert(mChunkHead.head != NULL);
        return reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead.head) + mChunkHead.capacity);
    }

    size_t getChunkRemain() const {
        jimi_assert(mChunkHead.head != NULL);
        return (mChunkHead.remain);
    }

    void * allocate(size_t size) {
        void * buffer;
        jimi_assert(mChunkHead.head != NULL);
        jimi_assert(mChunkHead.cursor != NULL);
        // Default alignment size is 8
        size = JIMI_ALIGNED_TO(size, kAlignmentSize);
        // If the chunk has enough space to allocate size bytes
        if (size <= mChunkHead.remain) {
            buffer = reinterpret_cast<char *>(mChunkHead.cursor);
            jimi_assert(buffer != NULL);

            //JIMI_PVOID_ADD(mChunkHead.cursor, size);
            mChunkHead.cursor = reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead.cursor) + size);

            mChunkHead.remain -= size;
            jimi_assert((ssize_t)mChunkHead.remain >= 0);
            return buffer;
        }
        else {
            jimi_assert(mChunkHead.capacity >= mChunkHead.remain);

#if !defined(JSONFX_ALLOW_ALLOC_BIGSIZE) || (JSONFX_ALLOW_ALLOC_BIGSIZE == 0)
            // The allocate size can not be greater than (kChunkCapacity - sizeof(ChunkInfo)).
            jimi_assert(size <= (kChunkCapacity - sizeof(ChunkInfo)));
            // Add a default capacity size chunk
            buffer = addNewChunk(size);
#else  /* !defined(JSONFX_ALLOW_ALLOC_BIGSIZE) */
            // Add a default capacity size chunk
            if (size <= (kChunkCapacity - sizeof(ChunkInfo))) {
                buffer = addNewChunk(size);
            }
            else {
                size_t newAllocSize = size + sizeof(ChunkInfo);
                newAllocSize = JIMI_ALIGNED_TO(newAllocSize, kAlignmentSize);
                if (mChunkHead.remain >= 512) {
                    // When remain >= 512
                    buffer = insertNewChunkToLast(newAllocSize, size);
                }
                else {
                    // When remain < 512
                    size_t newChunkCapacity = internal::RoundToPowerOf2(newAllocSize);
                    if ((newChunkCapacity - newAllocSize) >= 1024)
                        buffer = addNewChunk(newChunkCapacity, size);
                    else
                        buffer = insertNewChunkToLast(newAllocSize, size);
                }
            }
#endif  /* defined(JSONFX_ALLOW_ALLOC_BIGSIZE) */

            jimi_assert(mChunkHead.head != NULL);
            jimi_assert(mChunkHead.cursor != NULL);

            jimi_assert(buffer != NULL);
            return buffer;
        }
    }

    void * allocateLarge(size_t size) {
        size_t allocSize = size + sizeof(ChunkInfo);
        allocSize = JIMI_ALIGNED_TO(allocSize, kAlignmentSize);

        void * buffer = insertNewChunkToLast(allocSize, size);
        jimi_assert(mChunkHead.head != NULL);
        jimi_assert(mChunkHead.head->next != NULL);

        jimi_assert(buffer != NULL);
        return buffer;
    }

    void * reallocate(const void * ptr, size_t size, size_t new_size) {
        // Do not shrink if new size is smaller than original
        if (size >= new_size)
            return ptr;

        // Simply expand it if it is the last allocation and there is sufficient space
        if (ptr == reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead.head)
                + (mChunkHead.capacity - mChunkHead.remain) - size)) {
            size_t increment = static_cast<size_t>(new_size - size);
            increment = JIMI_ALIGNED_TO(increment, kAlignmentSize);
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
};

// Recover the packing alignment
#pragma pack(pop)

}  // namespace JsonFx

#endif  /* _JSONFX_SIMPLE_POOL_ALLOCATOR_H_ */
