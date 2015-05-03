
#ifndef _JSONFX_MEMBER_H_
#define _JSONFX_MEMBER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>
#include <iterator>

#include "JsonFx/Config.h"
#include "JsonFx/Internal/Traits.h"

namespace JsonFx {

/****************************************************************

// Forward declaration.
template <typename EncodingT = JSONFX_DEFAULT_ENCODING,
          typename PoolAllocatorT = DefaultPoolAllocator>
class BasicValue;

/****************************************************************/

#include "JsonFx/Detail/Reader-Fwd.h"

template <typename EncodingT = JSONFX_DEFAULT_ENCODING,
          typename PoolAllocatorT = DefaultPoolAllocator> 
class BasicMember
{
public:
    BasicValue<EncodingT, PoolAllocatorT> name;    //!< name of member (must be a string)
    BasicValue<EncodingT, PoolAllocatorT> value;   //!< value of member.
};

// Define default Member class type
typedef BasicMember<>   Member;

template <bool IsConst, typename EncodingT, typename PoolAllocatorT>
class BasicMemberIterator
    : public std::iterator<std::random_access_iterator_tag,
             typename internal::MaybeAddConst<IsConst, BasicMember<EncodingT, PoolAllocatorT> >::Type>
{
    friend class BasicValue<EncodingT, PoolAllocatorT>;
    template <bool, typename, typename> friend class BasicMemberIterator;

    typedef BasicMember<EncodingT, PoolAllocatorT>                      PlainType;
    typedef typename internal::MaybeAddConst<IsConst, PlainType>::Type  ValueType;
    typedef std::iterator<std::random_access_iterator_tag, ValueType>   BaseType;

public:
    //! Iterator type itself
    typedef BasicMemberIterator Iterator;

    //! Constant iterator type
    typedef BasicMemberIterator<true,  EncodingT, PoolAllocatorT>   ConstIterator;
    //! Non-constant iterator type
    typedef BasicMemberIterator<false, EncodingT, PoolAllocatorT>   NonConstIterator;

    //! Pointer to (const) GenericMember
    typedef typename BaseType::pointer         Pointer;
    //! Reference to (const) GenericMember
    typedef typename BaseType::reference       Reference;
    //! Signed integer type (e.g. \c ptrdiff_t)
    typedef typename BaseType::difference_type DifferenceType;

public:
    BasicMemberIterator() : mPtr() {}
    BasicMemberIterator(const NonConstIterator & it) : mPtr(it.mPtr) {}

    ~BasicMemberIterator() {}

    Iterator & operator ++()      { ++mPtr; return *this; }
    Iterator & operator --()      { --mPtr; return *this; }
    Iterator   operator ++(int)   { Iterator old(*this); ++mPtr; return old; }
    Iterator   operator --(int)   { Iterator old(*this); --mPtr; return old; }

    Iterator operator +(DifferenceType n) const { return Iterator(mPtr + n); }
    Iterator operator -(DifferenceType n) const { return Iterator(mPtr - n); }

    Iterator & operator +=(DifferenceType n) { mPtr += n; return *this; }
    Iterator & operator -=(DifferenceType n) { mPtr -= n; return *this; }

    bool operator ==(ConstIterator that) const { return mPtr == that.mPtr; }
    bool operator !=(ConstIterator that) const { return mPtr != that.mPtr; }
    bool operator <=(ConstIterator that) const { return mPtr <= that.mPtr; }
    bool operator >=(ConstIterator that) const { return mPtr >= that.mPtr; }
    bool operator < (ConstIterator that) const { return mPtr <  that.mPtr; }
    bool operator > (ConstIterator that) const { return mPtr >  that.mPtr; }

    Reference operator * () const { return *mPtr; }
    Pointer   operator ->() const { return  mPtr; }
    Reference operator [](DifferenceType n) const { return mPtr[n]; }

    //! Distance
    DifferenceType operator -(ConstIterator that) const { return mPtr - that.mPtr; }

private:
    //! Internal constructor from plain pointer
    explicit BasicMemberIterator(Pointer p) : mPtr(p) {}

    Pointer mPtr;
};

}  // namespace JsonFx

#endif  /* _JSONFX_MEMBER_H_ */
