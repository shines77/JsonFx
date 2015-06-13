
#ifndef _JSONFX_FAST_POOL_ALLOCATOR_H_
#define _JSONFX_FAST_POOL_ALLOCATOR_H_

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
class FastPoolAllocator : public PoolAllocator
{
public:
    typedef size_t      SizeType;
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
    //
    // Notice: For the allocated address aligned to kAlignmentSize bytes,
    //         the size of struct ChunkInfo must be setting for multiple of kAlignmentSize.
    //
    struct ChunkInfo {
        ChunkInfo * next;
        size_t      used;
        size_t      capacity;
#if !(defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) \
        || defined(__amd64__) || defined(__x86_64__))
        // Padding for align to 8 bytes in 32-bit mode
        size_t      padding1;
#endif  /* !(_M_X64 && _M_AMD64) */
    };

private:
    ChunkInfo * mChunkHead;
    void *      mUserBuffer;
    size_t      mUserBufSize;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
    size_t      mUsedTotal;
    size_t      mCapacityTotal;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */

    // The inner buffer on stack
    ALIGN_PREFIX(JSONFX_POOL_ALIGNMENT_SIZE)
    char        mInnerBuffer[kInnerChunkCapacity]
    ALIGN_SUFFIX(JSONFX_POOL_ALIGNMENT_SIZE);

public:
    FastPoolAllocator()
        : mChunkHead(NULL), mUserBuffer(NULL), mUserBufSize(0)
#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        , mUsedTotal(0), mCapacityTotal(kInnerChunkCapacity)
#endif
    {
        jimi_assert(kChunkCapacity >= kMinChunkCapacityThreshold);
        this->init();
    }

    FastPoolAllocator(void * userBuffer, size_t bufSize)
        : mChunkHead(NULL), mUserBuffer(userBuffer), mUserBufSize(bufSize)
#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        , mUsedTotal(0), mCapacityTotal(bufSize)
#endif
    {
        jimi_assert(kChunkCapacity >= kMinChunkCapacityThreshold);
        this->init(userBuffer, bufSize);
    }

    ~FastPoolAllocator() {
        this->destroy();
    }

    JIMI_FORCEINLINE
    void release() {
        if (this->kAutoRelease) {
            ChunkInfo * pChunkInfo = mChunkHead;
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
        this->release();
        // Reset used total counter
        mUsedTotal = 0;
        // Reset the first chunk info and counter info.
        this->init(mUserBuffer, mUserBufSize);
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
    FastPoolAllocator(const FastPoolAllocator & rhs);               /* = delete */
    //! Copy assignment operator is not permitted.
    FastPoolAllocator & operator =(const FastPoolAllocator & rhs);  /* = delete */

    JIMI_FORCEINLINE
    void destroy() {
        if (this->kAutoRelease) {
            // Release the chunk lists.
            this->release();
            mChunkHead = NULL;
        }
    }

    void internal_init() {
        mChunkHead      = NULL;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mUsedTotal      = 0;
        mCapacityTotal  = kInnerChunkCapacity;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */
    }

    void init() {
        if (kInnerChunkCapacity > kChunkCapacityLimit) {
            this->initInnerChunk();
            if (kInnerChunkCapacity <= kMinChunkCapacityThreshold)
                this->addNewChunk(0);
        }
        else {
            this->addNewChunk(0);
        }
    }

    void init(void * buffer, size_t size) {
        if (buffer != NULL) {
            if (size > kMinChunkCapacityThreshold) {
                initInnerChunk(buffer, size);
                return;
            }
        }
        this->init();
    }

    void initInnerChunk() {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(mInnerBuffer);

        newChunk->next     = NULL;
        newChunk->used     = sizeof(ChunkInfo);
        newChunk->capacity = kInnerChunkCapacity;

        mChunkHead = newChunk;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mCapacityTotal = kInnerChunkCapacity;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */  
    }

    void initInnerChunk(void * buffer, size_t size) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(buffer);

        newChunk->next     = NULL;
        newChunk->used     = sizeof(ChunkInfo);
        newChunk->capacity = size;

        mChunkHead = newChunk;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mCapacityTotal = size;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */  
    }

    void * addNewChunk(size_t nChunkCapacity, size_t size) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>
                    (AllocatorType::aligned_malloc(nChunkCapacity, kAlignmentSize));
        jimi_assert(newChunk != NULL);

        // Do not handle out-of-memory explicitly.
        newChunk->next      = mChunkHead;
        newChunk->used      = sizeof(ChunkInfo) + size;
        newChunk->capacity  = nChunkCapacity;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mUsedTotal      += mChunkHead->used;
        mCapacityTotal  += nChunkCapacity;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */  

        mChunkHead = newChunk;

        void * cursor = reinterpret_cast<void *>(newChunk + 1);
        return cursor;
    }

    void * insertNewChunkToLast(size_t nChunkCapacity, size_t size) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>
                    (AllocatorType::aligned_malloc(nChunkCapacity, kAlignmentSize));
        jimi_assert(newChunk != NULL);

        // Do not handle out-of-memory explicitly.
        newChunk->next      = mChunkHead->next;
        newChunk->used      = sizeof(ChunkInfo) + size;
        newChunk->capacity  = nChunkCapacity;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mUsedTotal      += sizeof(ChunkInfo) + size;
        mCapacityTotal  += nChunkCapacity;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */

        mChunkHead->next = newChunk;

        void * cursor = reinterpret_cast<void *>(newChunk + 1);
        return cursor;
    }

public:
#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
    size_t getUsed() const {
        return (mUsedTotal + mChunkHead->used);
    }

    size_t getCapacity() const {
        return mCapacityTotal;
    }
#else  /* !defined(JSONFX_ALLOCATOR_USE_PROFILE) */
    size_t getUsed() const {
        return 0;
    }

    size_t getCapacity() const {
        return 0;
    }
#endif  /* defined(JSONFX_ALLOCATOR_USE_PROFILE) */

    void * addNewChunk(size_t size) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>
                    (AllocatorType::aligned_malloc(kChunkCapacity, kAlignmentSize));
        jimi_assert(newChunk != NULL);

        // Do not handle out-of-memory explicitly.
        newChunk->next      = mChunkHead;
        newChunk->used      = sizeof(ChunkInfo) + size;
        newChunk->capacity  = kChunkCapacity;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        if (mChunkHead != NULL) {
            mUsedTotal  += mChunkHead->used;
        }
        mCapacityTotal  += kChunkCapacity;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */

        mChunkHead = newChunk;

        void * cursor = reinterpret_cast<void *>(newChunk + 1);
        return cursor;
    }

    inline void * addNewChunkAndSkip(size_t skipSize) {
        return this->addNewChunkAndSkip(skipSize, 0);
    }

    inline void * addNewChunkAndSkip(size_t skipSize, size_t reserveSize) {
        jimi_assert(mChunkHead != NULL);
        void * cursor;
        if ((skipSize + reserveSize) <= kChunkCapacity) {
            cursor = this->addNewChunk(0);
            return reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + skipSize);
        }
        else {
            cursor = this->addNewChunk(skipSize + reserveSize, 0);
            return reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + skipSize);
        }
    }

    void * skip(size_t skipSize) {
        jimi_assert(mChunkHead != NULL);
        if (skipSize <= (mChunkHead->capacity - mChunkHead->used)) {
            return reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead) + mChunkHead->used + skipSize);
        }
        else {
            void * cursor = this->addNewChunk(0);
            return reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + skipSize);
        }
    }

    void * skip(size_t skipSize, size_t reserveSize) {
        jimi_assert(mChunkHead != NULL);
        if ((skipSize + reserveSize) <= (mChunkHead->capacity - mChunkHead->used)) {
            return reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead) + mChunkHead->used + skipSize);
        }
        else {
            void * cursor = this->addNewChunk(0);
            return reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + skipSize);
        }
    }

    void * getChunkCursor() const {
        jimi_assert(mChunkHead != NULL);
        return reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead) + mChunkHead->used);
    }

    void * getChunkBottom() const {
        jimi_assert(mChunkHead != NULL);
        return reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead) + mChunkHead->capacity);
    }

    size_t getChunkRemain() const {
        jimi_assert(mChunkHead != NULL);
        return (mChunkHead->capacity - mChunkHead->used);
    }

    void * allocate(size_t size) {
        void * buffer;
        size_t remain;
        jimi_assert(mChunkHead != NULL);
        // Default alignment size is 8
        size = JIMI_ALIGNED_TO(size, kAlignmentSize);
        remain = mChunkHead->capacity - mChunkHead->used;
        // If the chunk has enough space to allocate size bytes
        if (size <= remain) {
            buffer = reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead) + mChunkHead->used);
            jimi_assert(buffer != NULL);

            mChunkHead->used += size;
            jimi_assert(mChunkHead->used <= mChunkHead->capacity);
            return buffer;
        }
        else {
#if !defined(JSONFX_ALLOW_ALLOC_BIGSIZE) || (JSONFX_ALLOW_ALLOC_BIGSIZE == 0)
            // The allocate size can not be greater than (kChunkCapacity - sizeof(ChunkInfo)).
            jimi_assert(size <= (kChunkCapacity - sizeof(ChunkInfo)));
            // Add a default capacity size chunk
            buffer = this->addNewChunk(size);
#else  /* !defined(JSONFX_ALLOW_ALLOC_BIGSIZE) */
            // Add a default capacity size chunk
            if (size <= (kChunkCapacity - sizeof(ChunkInfo))) {
                buffer = this->addNewChunk(size);
            }
            else {
                size_t newAllocSize = size + sizeof(ChunkInfo);
                newAllocSize = JIMI_ALIGNED_TO(newAllocSize, kAlignmentSize);
                if (remain >= 512) {
                    // When remain >= 512
                    buffer = this->insertNewChunkToLast(newAllocSize, size);
                }
                else {
                    // When remain < 512
                    size_t newChunkCapacity = internal::RoundToPowerOf2(newAllocSize);
                    if ((newChunkCapacity - newAllocSize) >= 1024)
                        buffer = this->addNewChunk(newChunkCapacity, size);
                    else
                        buffer = this->insertNewChunkToLast(newAllocSize, size);
                }
            }
#endif  /* defined(JSONFX_ALLOW_ALLOC_BIGSIZE) */
            jimi_assert(mChunkHead != NULL);
            jimi_assert(mChunkHead->next != NULL);

            jimi_assert(buffer != NULL);
            return buffer;
        }
    }

    void * allocateLarge(size_t size) {
        size_t allocSize = size + sizeof(ChunkInfo);
        allocSize = JIMI_ALIGNED_TO(allocSize, kAlignmentSize);

        void * buffer = this->insertNewChunkToLast(allocSize, size);
        jimi_assert(mChunkHead != NULL);
        jimi_assert(mChunkHead->next != NULL);

        jimi_assert(buffer != NULL);
        return buffer;
    }

    void * reallocate(const void * ptr, size_t size, size_t new_size) {
        // Do not shrink if new size is smaller than original.
        if (size >= new_size)
            return ptr;

        // Simply expand it if it is the last allocation and there is sufficient space.
        if (ptr == reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead)
                + mChunkHead->used - size)) {
            // Here always new_size > size.
            size_t increment = new_size - size;
            increment = JIMI_ALIGNED_TO(increment, kAlignmentSize);
            if (mChunkHead->used + increment <= mChunkHead->capacity) {
                mChunkHead->used += increment;
                return reinterpret_cast<void *>(const_cast<void *>(ptr));
            }
        }

        // Realloc process: allocate and copy memory, do not free original buffer.
        void * newBuffer = this->allocate(new_size);
        jimi_assert(newBuffer != NULL);     // Do not handle out-of-memory explicitly.
        return reinterpret_cast<void *>(std::memcpy(newBuffer, ptr, size));
    }

    static void deallocate(void * ptr) { (void)ptr; }                           /* Do nothing! */
    static void deallocate(void * ptr, size_t size) { (void)ptr; (void)size; }  /* Do nothing! */
};

// Recover the packing alignment
#pragma pack(pop)

}  // namespace JsonFx

#endif  /* _JSONFX_FAST_POOL_ALLOCATOR_H_ */
