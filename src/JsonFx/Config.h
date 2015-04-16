
#ifndef _JSONFX_CONFIG_H_
#define _JSONFX_CONFIG_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//! Default system pagesize
#define JSONFX_DEFAULT_PAGESIZE     4096

//! Default memory pool inner chunk capacity
#define JSONFX_POOL_INNER_BUFSIZE   8192

#define ALIGN_PREFIX(N)             __declspec(align(N))
#define ALIGN_SUFFIX(N)

#ifndef _Ch
#ifndef __cplusplus
#define _Ch(c)        ((CharType)c)
#else   /* !__cplusplus */
#define _Ch(c)        static_cast<CharType>(c)
#endif  /* __cplusplus */
#endif  /* _Ch */

#ifndef _Tx
#ifdef _UNICODE
#define _Tx(text)      L ## text
#else   /* !_UNICODE */
#define _Tx(text)      text
#endif  /* _UNICODE */
#endif  /* _Tx */

namespace JsonFx {

/* define in "JsonFx/Allcator.h" */

// Forward declaration.
class TrivialAllocator;

// Forward declaration.
template <size_t ChunkCapacity, size_t InnerChunkCapacity, typename Allocator>
class StackPoolAllocator;

// Forward declaration.
template <size_t ChunkCapacity, size_t InnerChunkCapacity, typename Allocator>
class StdPoolAllocator;

// Forward declaration.
template <size_t ChunkCapacity, size_t InnerChunkCapacity, typename Allocator>
class SimplePoolAllocator;

// Forward declaration.
template <size_t ChunkCapacity, size_t InnerChunkCapacity, typename Allocator>
class FastPoolAllocator;

//! Default StdPoolAllocator chunk capacity (Recommended settings for multiple systems PageSize.)
static const size_t gDefaultChunkCapacity = 16 * JSONFX_DEFAULT_PAGESIZE;

//! Default StdPoolAllocator inner chunk capacity (On stack, recommended settings for less than or equal 64KB.)
static const size_t gDefaultInnerChunkCapacity = JSONFX_POOL_INNER_BUFSIZE;

//! Default encoding chartype
#define JSONFX_DEFAULT_CHARTYPE     char

// Define default char type
typedef JSONFX_DEFAULT_CHARTYPE     DefaultCharType;

// Define default stack allocator
typedef TrivialAllocator            DefaultAllocator;

#if 0
typedef StdPoolAllocator<gDefaultChunkCapacity,
                         gDefaultInnerChunkCapacity,
                         DefaultAllocator>          DefaultPoolAllocator;
#elif 0
typedef SimplePoolAllocator<gDefaultChunkCapacity,
                            gDefaultInnerChunkCapacity,
                            DefaultAllocator>       DefaultPoolAllocator;
#elif 1
typedef FastPoolAllocator<gDefaultChunkCapacity,
                          gDefaultInnerChunkCapacity,
                          DefaultAllocator>         DefaultPoolAllocator;
#else
typedef StackPoolAllocator<gDefaultChunkCapacity,
                           gDefaultInnerChunkCapacity,
                           DefaultAllocator>        DefaultPoolAllocator;
#endif

}  // namespace JsonFx

#endif  /* !_JSONFX_CONFIG_H_ */
