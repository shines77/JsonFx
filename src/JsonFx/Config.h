
#ifndef _JSONFX_CONFIG_H_
#define _JSONFX_CONFIG_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

// Define default char
#define JSONFX_DEFAULT_CHARTYPE     char

#define JSONFX_DEFAULT_PAGESIZE     4096

namespace JsonFx {

/* define in "JsonFx/Allcator.h" */

// Forward declaration.
class CrtAllocator;

// Forward declaration.
template <size_t ChunkCapacity, typename Allocator>
class StackPoolAllocator;

// Forward declaration.
template <size_t ChunkCapacity, typename Allocator>
class MemoryPoolAllocator;

// Forward declaration.
template <size_t ChunkCapacity, typename Allocator>
class SimpleMemoryPoolAllocator;

// Forward declaration.
template <size_t ChunkCapacity, typename Allocator>
class FastMemoryPoolAllocator;

//! Default MemoryPoolAllocator chunk capacity (Recommended settings for multiple systems PageSize)
static const size_t kDefaultChunkCapacity = 16 * JSONFX_DEFAULT_PAGESIZE;

// Define default char type
typedef JSONFX_DEFAULT_CHARTYPE     DefaultCharType;

// Define default stack allocator
typedef CrtAllocator                DefaultAllocator;

#if 1
typedef StackPoolAllocator<kDefaultChunkCapacity, DefaultAllocator>         DefaultPoolAllocator;
#elif 0
typedef MemoryPoolAllocator<kDefaultChunkCapacity, DefaultAllocator>        DefaultPoolAllocator;
#elif 0
typedef SimpleMemoryPoolAllocator<kDefaultChunkCapacity, DefaultAllocator>  DefaultPoolAllocator;
#else
typedef FastMemoryPoolAllocator<kDefaultChunkCapacity, DefaultAllocator>    DefaultPoolAllocator;
#else

#endif

}  // namespace JsonFx

#endif  /* !_JSONFX_CONFIG_H_ */
