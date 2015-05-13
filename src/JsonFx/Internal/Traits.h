
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

///////////////////////////////////////////////////////////////////////////////
// IsSame, IsConst, IsMoreConst, IsPointer
//
template <typename T, typename U> struct IsSame : FalseType {};
template <typename T> struct IsSame<T, T> : TrueType {};

template <typename T> struct IsConst : FalseType {};
template <typename T> struct IsConst<const T> : TrueType {};

template <typename CT, typename T>
struct IsMoreConst
    : AndExpr<IsSame<typename RemoveConst<CT>::Type, typename RemoveConst<T>::Type>,
              BoolType<IsConst<CT>::Value >= IsConst<T>::Value> >::Type {};

template <typename T> struct IsPointer : FalseType {};
template <typename T> struct IsPointer<T*> : TrueType {};

#ifndef jimi_staic_assert
//#define jimi_staic_assert(expr, ...)    static_assert((expr), __VA_ARGS__)
#define jimi_staic_assert(expr, ...)
#endif

///////////////////////////////////////////////////////////////////////////////
// IsBaseOf
//
#if JSONFX_HAS_CXX11_TYPETRAITS

template <typename B, typename D> struct IsBaseOf
    : BoolType< ::std::is_base_of<B,D>::value> {};

#else // simplified version adopted from Boost

template<typename B, typename D> struct IsBaseOfImpl {
    jimi_staic_assert(sizeof(B) != 0);
    jimi_staic_assert(sizeof(D) != 0);

    typedef char (&Yes)[1];
    typedef char (&No) [2];

    template <typename T>
    static Yes Check(const D *, T);
    static No  Check(const B *, int);

    struct Host {
        operator const B *() const;
        operator const D *();
    };

    enum { Value = (sizeof(Check(Host(), 0)) == sizeof(Yes)) };
};

template <typename B, typename D> struct IsBaseOf
    : OrExpr<IsSame<B, D>, BoolExpr<IsBaseOfImpl<B, D> > >::Type {};

#endif // JSONFX_HAS_CXX11_TYPETRAITS


//////////////////////////////////////////////////////////////////////////
// EnableIf / DisableIf
//
template <bool Condition, typename T = void> struct EnableIfCond  { typedef T Type; };
template <typename T> struct EnableIfCond<false, T> { /* empty */ };

template <bool Condition, typename T = void> struct DisableIfCond { typedef T Type; };
template <typename T> struct DisableIfCond<true, T> { /* empty */ };

template <typename Condition, typename T = void>
struct EnableIf : EnableIfCond<Condition::Value, T> {};

template <typename Condition, typename T = void>
struct DisableIf : DisableIfCond<Condition::Value, T> {};

}  // namespace internal

}  // namespace jimi

#endif  /* _JSONFX_INTERNAL_TRAITS_H_ */
