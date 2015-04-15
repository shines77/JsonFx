
#ifndef _JSONFX_ALLOCATOR_H_
#define _JSONFX_ALLOCATOR_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <memory>

#include "JsonFx/Config.h"
#include "JsonFx/internal/Utils.h"
#include "JsonFx/CharSet.h"
#include "JsonFx/Value.h"

#include "jimi/basic/archdef.h"
#include "jimi/basic/stddef.h"
#include "jimi/basic/assert.h"

/* Whether use profile codes when use JsonFx allocators? */
#define JSONFX_ALLOCATOR_USE_PROFILE    1

/* If it is 0, don't allow malloc size more than the default chunk size. */
#define JSONFX_ALLOW_ALLOC_BIGSIZE      0

/* Default malloc alignment size. */
#define JSONFX_POOL_ALIGNMENT_SIZE      8

namespace JsonFx {

/* Just define for inheritance. */
class StackAllocator {
public:
    static const bool kNeedFree = false;
};

/* Just define for inheritance. */
class PoolAllocator {
public:
    static const bool   kNeedFree           = false;
    static const bool   kAutoRelease        = false;
    static const size_t kChunkCapacity      = 0;
    static const size_t kInnerChunkCapacity = 0;
    static const size_t kAlignmentSize      = JSONFX_POOL_ALIGNMENT_SIZE;
};

// C-RunTime Allocator
class TrivialAllocator : public StackAllocator
{
public:
    static const bool kNeedFree = true;

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

template <size_t ChunkCapacity = kDefaultChunkCapacity,
          size_t InnerChunkCapacity = kDefaultInnerChunkCapacity,
          typename Allocator = DefaultAllocator>
class StackPoolAllocator : public PoolAllocator
{
public:
    typedef Allocator AllocatorType;

    static const bool   kNeedFree           = false;
    static const bool   kAutoRelease        = false;
    static const size_t kChunkCapacity      = ChunkCapacity;
    static const size_t kInnerChunkCapacity = InnerChunkCapacity;
    static const size_t kAlignmentSize      = JSONFX_POOL_ALIGNMENT_SIZE;

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
          size_t InnerChunkCapacity = kDefaultInnerChunkCapacity,
          typename Allocator = DefaultAllocator>
class MemoryPoolAllocator : public PoolAllocator
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

public:
    MemoryPoolAllocator() : mChunkHead(NULL) {
        init();
    }
    ~MemoryPoolAllocator() {
        destroy();
    }

    void destroy() {
        if (this->kAutoRelease) {
            ChunkHead * pChunkHead = mChunkHead;
            while (pChunkHead != NULL) {
                ChunkHead * next = pChunkHead->next;
                if (next != NULL) {
                    AllocatorType::free(pChunkHead);
                }
                pChunkHead = next;
            }
            mChunkHead = NULL;
        }
    }

private:
    //! Copy constructor is not permitted.
    MemoryPoolAllocator(const MemoryPoolAllocator & rhs);               /* = delete */
    //! Copy assignment operator is not permitted.
    MemoryPoolAllocator& operator =(const MemoryPoolAllocator & rhs);   /* = delete */

    void init() {
        if (kInnerChunkCapacity > sizeof(ChunkHead))
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

private:
    // The chunk head info
    ChunkHead * mChunkHead;
    // The inner buffer on stack
    char        mInnerBuffer[kInnerChunkCapacity];
};

template <size_t ChunkCapacity = kDefaultChunkCapacity,
          size_t InnerChunkCapacity = kDefaultInnerChunkCapacity,
          typename Allocator = DefaultAllocator>
class SimpleMemoryPoolAllocator : public PoolAllocator
{
public:
    typedef Allocator   AllocatorType;

    static const bool   kNeedFree           = false;
    static const bool   kAutoRelease        = true;
    static const size_t kChunkCapacity      = ChunkCapacity;
    static const size_t kInnerChunkCapacity = InnerChunkCapacity;
    static const size_t kAlignmentSize      = JSONFX_POOL_ALIGNMENT_SIZE;

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

public:
    SimpleMemoryPoolAllocator() {
        init();
    }

    ~SimpleMemoryPoolAllocator() {
        destroy();
    }

    void destroy() {
        if (this->kAutoRelease) {
            ChunkInfo * pChunkInfo = mChunkHead.head;
            while (pChunkInfo != NULL) {
                ChunkInfo * next = pChunkInfo->next;
                if (next != NULL) {
                    AllocatorType::free(pChunkInfo);
                }
                pChunkInfo = next;
            }
            mChunkHead.head = NULL;
        }
    }

private:
    //! Copy constructor is not permitted.
    SimpleMemoryPoolAllocator(const SimpleMemoryPoolAllocator & rhs);               /* = delete */
    //! Copy assignment operator is not permitted.
    SimpleMemoryPoolAllocator& operator =(const SimpleMemoryPoolAllocator & rhs);   /* = delete */

    void init() {
        mChunkHead.cursor   = NULL;
        mChunkHead.remain   = 0;
        mChunkHead.capacity = 0;
        mChunkHead.head     = NULL;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mChunkHead.usedTotal        = 0;
        mChunkHead.capacityTotal    = 0;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */

        if (kInnerChunkCapacity > sizeof(ChunkInfo))
            initInnerChunk();
        else
            addNewChunk(0);
    }

    void initInnerChunk() {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(mInnerBuffer);

        newChunk->next      = NULL;
        newChunk->capacity  = kInnerChunkCapacity;

        mChunkHead.cursor   = reinterpret_cast<void *>(newChunk + 1);
        mChunkHead.remain   = kInnerChunkCapacity - sizeof(ChunkInfo);
        mChunkHead.capacity = kInnerChunkCapacity;

        mChunkHead.head = newChunk;
    }

    void * addNewChunk(size_t size) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(AllocatorType::malloc(kChunkCapacity));
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

    void * addNewChunk(size_t nChunkCapacity, size_t size) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(AllocatorType::malloc(nChunkCapacity));
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
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(AllocatorType::malloc(nChunkCapacity));
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
                if ((newChunkCapacity - newAllocSize) > mChunkHead.remain)
                    buffer = addNewChunk(newChunkCapacity, size);
                else
                    buffer = insertNewChunkToLast(newAllocSize, size);
            }
#endif  /* defined(JSONFX_ALLOW_ALLOC_BIGSIZE) */

            jimi_assert(mChunkHead.head != NULL);
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

private:
    // The chunk head info
    ChunkHead   mChunkHead;
    // The inner buffer on stack
    char        mInnerBuffer[kInnerChunkCapacity];
};

template <size_t ChunkCapacity = kDefaultChunkCapacity,
          size_t InnerChunkCapacity = kDefaultInnerChunkCapacity,
          typename Allocator = DefaultAllocator>
class FastMemoryPoolAllocator : public PoolAllocator
{
public:
    typedef Allocator   AllocatorType;

    static const bool   kNeedFree           = false;
    static const bool   kAutoRelease        = true;
    static const size_t kChunkCapacity      = ChunkCapacity;
    static const size_t kInnerChunkCapacity = InnerChunkCapacity;
    static const size_t kAlignmentSize      = JSONFX_POOL_ALIGNMENT_SIZE;

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

public:
    FastMemoryPoolAllocator() : mChunkHead(NULL)
#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        , mUsedTotal(0), mCapacityTotal(kInnerChunkCapacity)
#endif
    {
        init();
    }

    ~FastMemoryPoolAllocator() {
        destroy();
    }

    void destroy() {
        if (this->kAutoRelease) {
            ChunkInfo * pChunkInfo = mChunkHead;
            while (pChunkInfo != NULL) {
                ChunkInfo * next = pChunkInfo->next;
                if (next != NULL) {
                    AllocatorType::aligned_free(pChunkInfo);
                }
                pChunkInfo = next;
            }
            mChunkHead = NULL;
        }
    }

private:
    //! Copy constructor is not permitted.
    FastMemoryPoolAllocator(const FastMemoryPoolAllocator & rhs);               /* = delete */
    //! Copy assignment operator is not permitted.
    FastMemoryPoolAllocator& operator =(const FastMemoryPoolAllocator & rhs);   /* = delete */

    void internal_init() {
        mChunkHead      = NULL;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
        mUsedTotal      = 0;
        mCapacityTotal  = kInnerChunkCapacity;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */
    }

    void init() {
        if (kInnerChunkCapacity > sizeof(ChunkInfo))
            initInnerChunk();
        else
            addNewChunk(0);
    }

    void initInnerChunk() {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(mInnerBuffer);

        newChunk->next     = NULL;
        newChunk->used     = sizeof(ChunkInfo);
        newChunk->capacity = kInnerChunkCapacity;

        mChunkHead = newChunk;
    }

    void * addNewChunk(size_t nChunkCapacity, size_t size) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(AllocatorType::aligned_malloc(nChunkCapacity, kAlignmentSize));
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
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(AllocatorType::aligned_malloc(nChunkCapacity, kAlignmentSize));
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
    void * addNewChunk(size_t size) {
        ChunkInfo * newChunk = reinterpret_cast<ChunkInfo *>(AllocatorType::aligned_malloc(kChunkCapacity, kAlignmentSize));
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

    void * reserve(size_t skipSize) {
        jimi_assert(mChunkHead != NULL);
        if (skipSize <= (mChunkHead->capacity - mChunkHead->used)) {
            return reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead) + mChunkHead->used + skipSize);
        }
        else {
            void * cursor = addNewChunk(0);
            return reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + skipSize);
        }
    }

    void * reserve(size_t skipSize, size_t reserveSize) {
        jimi_assert(mChunkHead != NULL);
        if ((skipSize + reserveSize) <= (mChunkHead->capacity - mChunkHead->used)) {
            return reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead) + mChunkHead->used + skipSize);
        }
        else {
            void * cursor = addNewChunk(0);
            return reinterpret_cast<void *>(reinterpret_cast<char *>(cursor) + skipSize);
        }
    }

    inline void * fastReserve(size_t skipSize) {
        return fastReserve(skipSize, 0);
    }

    inline void * fastReserve(size_t skipSize, size_t reserveSize) {
        jimi_assert(mChunkHead != NULL);
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
        jimi_assert(mChunkHead != NULL);
        // Default alignment size is 8
        size = JIMI_ALIGNED_TO(size, kAlignmentSize);
        // If the chunk has enough space to allocate size bytes
        if (mChunkHead->used + size <= mChunkHead->capacity) {
            buffer = reinterpret_cast<char *>(mChunkHead) + mChunkHead->used;
            jimi_assert(buffer != NULL);

            mChunkHead->used += size;
            jimi_assert(mChunkHead->used <= mChunkHead->capacity);
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
                if ((newChunkCapacity - newAllocSize) > (mChunkHead->capacity - mChunkHead->used))
                    buffer = addNewChunk(newChunkCapacity, size);
                else
                    buffer = insertNewChunkToLast(newAllocSize, size);
            }
#endif  /* defined(JSONFX_ALLOW_ALLOC_BIGSIZE) */

            jimi_assert(mChunkHead != NULL);
            jimi_assert(mChunkHead->next != NULL);

            jimi_assert(buffer != NULL);
            return buffer;
        }
    }

    void * reallocate(const void * ptr, size_t size, size_t new_size) {
        // Do not shrink if new size is smaller than original
        if (size >= new_size)
            return ptr;

        // Simply expand it if it is the last allocation and there is sufficient space
        if (ptr == reinterpret_cast<void *>(reinterpret_cast<char *>(mChunkHead)
                + mChunkHead->used - size)) {
            size_t increment = static_cast<size_t>(new_size - size);
            increment = JIMI_ALIGNED_TO(increment, kAlignmentSize);
            if (mChunkHead->used + increment <= mChunkHead->capacity) {
                mChunkHead->used += increment;
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
    ChunkInfo * mChunkHead;

#if defined(JSONFX_ALLOCATOR_USE_PROFILE) && (JSONFX_ALLOCATOR_USE_PROFILE != 0)
    size_t      mUsedTotal;
    size_t      mCapacityTotal;
#endif  /* JSONFX_ALLOCATOR_USE_PROFILE */

    // The inner buffer on stack
    ALIGN_PREFIX(JSONFX_POOL_ALIGNMENT_SIZE)
    char        mInnerBuffer[kInnerChunkCapacity]
    ALIGN_SUFFIX(JSONFX_POOL_ALIGNMENT_SIZE);
};

}  // namespace JsonFx

#endif  /* !_JSONFX_ALLOCATOR_H_ */
