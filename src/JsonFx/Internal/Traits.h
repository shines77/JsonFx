
#ifndef _JSONFX_INTERNAL_TRAITS_H_
#define _JSONFX_INTERNAL_TRAITS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

namespace JsonFx {

namespace internal {

// Helper to wrap/convert arbitrary types to void, useful for arbitrary type matching
template <typename T>
struct Void {
    typedef void Type;
};

///////////////////////////////////////////////////////////////////////////////
// BoolType, TrueType, FalseType
//
template <bool Cond>
struct BoolType {
    static const bool Value = Cond;
    typedef BoolType Type;
};

typedef BoolType<true>  TrueType;
typedef BoolType<false> FalseType;

///////////////////////////////////////////////////////////////////////////////
// SelectIf, BoolExpr, NotExpr, AndExpr, OrExpr
//
template <bool C>
struct SelectIfImpl {
    template <typename T1, typename T2>
    struct Apply { typedef T1 Type; };
};

template <>
struct SelectIfImpl<false> {
    template <typename T1, typename T2>
    struct Apply { typedef T2 Type; };
};

template <bool C, typename T1, typename T2>
struct SelectIfCond : SelectIfImpl<C>::template Apply<T1,T2> {};

template <typename C, typename T1, typename T2>
struct SelectIf : SelectIfCond<C::Value, T1, T2> {};

template <bool Cond1, bool Cond2>
struct AndExprCond : FalseType {};

template <>
struct AndExprCond<true, true> : TrueType {};

template <bool Cond1, bool Cond2>
struct OrExprCond : TrueType {};

template <>
struct OrExprCond<false, false> : FalseType {};

template <typename C>
struct BoolExpr : SelectIf<C,TrueType,FalseType>::Type {};

template <typename C>
struct NotExpr  : SelectIf<C,FalseType,TrueType>::Type {};

template <typename C1, typename C2>
struct AndExpr  : AndExprCond<C1::Value, C2::Value>::Type {};

template <typename C1, typename C2>
struct OrExpr   : OrExprCond<C1::Value, C2::Value>::Type {};

///////////////////////////////////////////////////////////////////////////////
// AddConst, MaybeAddConst, RemoveConst
//
template <typename T>
struct AddConst {
    typedef const T Type;
};

template <bool IsConstify, typename T>
struct MaybeAddConst : SelectIfCond<IsConstify, const T, T> {};

template <typename T>
struct RemoveConst {
    typedef T Type;
};

template <typename T>
struct RemoveConst<const T> {
    typedef T Type;
};

}  // namespace internal

}  // namespace jimi

#endif  /* _JSONFX_INTERNAL_TRAITS_H_ */
