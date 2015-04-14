
#ifndef _JSONFX_CONFIG_H_
#define _JSONFX_CONFIG_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//! Default system pagesize
#define JSONFX_DEFAULT_PAGESIZE     4096

//! Default memory pool inner chunk capacity
#define JSONFX_POOL_INNER_BUFSIZE   8192

namespace JsonFx {

/* define in "JsonFx/Allcator.h" */

// Forward declaration.
class TrivialAllocator;

// Forward declaration.
template <size_t ChunkCapacity, size_t InnerChunkCapacity, typename Allocator>
class StackPoolAllocator;

// Forward declaration.
template <size_t ChunkCapacity, size_t InnerChunkCapacity, typename Allocator>
class MemoryPoolAllocator;

// Forward declaration.
template <size_t ChunkCapacity, size_t InnerChunkCapacity, typename Allocator>
class SimpleMemoryPoolAllocator;

// Forward declaration.
template <size_t ChunkCapacity, size_t InnerChunkCapacity, typename Allocator>
class FastMemoryPoolAllocator;

//! Default MemoryPoolAllocator chunk capacity (Recommended settings for multiple systems PageSize.)
static const size_t kDefaultChunkCapacity = 16 * JSONFX_DEFAULT_PAGESIZE;

//! Default MemoryPoolAllocator inner chunk capacity (On stack, recommended settings for less than or equal 64KB.)
static const size_t kDefaultInnerChunkCapacity = JSONFX_POOL_INNER_BUFSIZE;

//! Default encoding chartype
#define JSONFX_DEFAULT_CHARTYPE     char

// Define default char type
typedef JSONFX_DEFAULT_CHARTYPE     DefaultCharType;

// Define default stack allocator
typedef TrivialAllocator            DefaultAllocator;

#if 0
typedef MemoryPoolAllocator<kDefaultChunkCapacity,
                            kDefaultInnerChunkCapacity,
                            DefaultAllocator>           DefaultPoolAllocator;
#elif 0
typedef SimpleMemoryPoolAllocator<kDefaultChunkCapacity,
                                  kDefaultInnerChunkCapacity,
                                  DefaultAllocator>     DefaultPoolAllocator;
#elif 1
typedef FastMemoryPoolAllocator<kDefaultChunkCapacity,
                                kDefaultInnerChunkCapacity,
                                DefaultAllocator>       DefaultPoolAllocator;
#else
typedef StackPoolAllocator<kDefaultChunkCapacity,
                           kDefaultInnerChunkCapacity,
                           DefaultAllocator>            DefaultPoolAllocator;
#endif

}  // namespace JsonFx

#endif  /* !_JSONFX_CONFIG_H_ */
