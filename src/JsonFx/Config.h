
#ifndef _JSONFX_CONFIG_H_
#define _JSONFX_CONFIG_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

// Define default char
#define JSONFX_DEFAULT_CHARTYPE     char

namespace JsonFx {

// Forward declaration.
class CrtAllocator;

// Forward declaration.
template <size_t ChunkCapacity, typename BaseAllocator>
class MemoryPoolAllocator;

//! Default chunk capacity (Recommended settings for multiple systems PageSize)
static const size_t kDefaultChunkCapacity = 16 * 4096;

// Define default char type
typedef JSONFX_DEFAULT_CHARTYPE     DefaultCharType;

typedef MemoryPoolAllocator<kDefaultChunkCapacity, CrtAllocator> DefaultAllocator;

}  // namespace JsonFx

#endif  /* !_JSONFX_CONFIG_H_ */
