
#ifndef _JSONFX_DETAIL_VALUE_FWD_H_
#define _JSONFX_DETAIL_VALUE_FWD_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(_JSONFX_VALUE_H_) || defined(_JSONFX_MEMBER_H_)

// Note: This file must include in namespace JsonFx inner.

// Forward declaration.
template <typename EncodingT = JSONFX_DEFAULT_ENCODING,
          typename PoolAllocatorT = DefaultPoolAllocator>
class BasicValue;

#else

#error This file only can use in JsonFx/Value.h or JsonFx/Member.h .

#endif

#endif  /* _JSONFX_DETAIL_VALUE_FWD_H_ */
