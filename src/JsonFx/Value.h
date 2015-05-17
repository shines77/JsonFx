
#ifndef _JSONFX_VALUE_H_
#define _JSONFX_VALUE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "jimi/basic/stdint.h"
#include "jimi/basic/stdsize.h"

#include "JsonFx/Config.h"
#include "JsonFx/Allocator.h"
#include "JsonFx/StringRef.h"
#include "JsonFx/Member.h"

#include "JsonFx/Internal/Traits.h"

// Just for temporary test!
#ifdef  jimi_assert
#undef  jimi_assert
#define jimi_assert(...)    ((void *)0)
#endif

namespace JsonFx {

enum ValueType {
    // Base value type
    kObjectType,
    kStringType,
    kNumberType,
    kTrueType,
    kFalseType,
    kArrayType,
    kNullType,

    // Last value type
    kMaxValueType = kNullType + 1,

    // Extend value type
    kBoolType = kMaxValueType,
    kIntegerType,
    kDoubleType,
    kFloatType,

    kMaxValueTypeEx,
};

enum ValueTypeMask {
    kBoolMask           = 0x00000100,
    kInt8Mask           = 0x00000200,
    kUInt8Mask          = 0x00000400,
    kInt16Mask          = 0x00000800,
    kUInt16Mask         = 0x00001000,
    kInt32Mask          = 0x00002000,
    kUInt32Mask         = 0x00004000,
    kInt64Mask          = 0x00008000,
    kUInt64Mask         = 0x00010000,

    kIntegerMask        = 0x00020000,
    kFloatMask          = 0x00040000,
    kDoubleMask         = 0x00080000,
    kNumberMask         = 0x00100000,

    kStringMask         = 0x00200000,
    kCopyStringMask     = 0x00400000,
    kInlineStringMask   = 0x00800000,

    kNumberBoolMask     = kNumberMask | kIntegerMask | kBoolMask,

    kBoolFlags          = kNumberMask | kIntegerMask | kBoolMask   | kBoolType,
    kTrueFlags          = kNumberMask | kIntegerMask | kBoolMask   | kTrueType,
    kFalseFlags         = kNumberMask | kIntegerMask | kBoolMask   | kFalseType,

    kNumberIntegerMask  = kNumberMask | kIntegerMask,

    kIntegerFlags       = kNumberMask | kIntegerMask               | kIntegerType,
    kIntFlags           = kNumberMask | kIntegerMask | kInt32Mask  | kNumberType,
    kUIntFlags          = kNumberMask | kIntegerMask | kUInt32Mask | kNumberType,

    kInt64Flags         = kNumberMask | kIntegerMask | kInt64Mask  | kNumberType,
    kUInt64Flags        = kNumberMask | kIntegerMask | kUInt64Mask | kNumberType,

    kFloatFlags         = kNumberMask | kFloatMask   | kNumberType,
    kDoubleFlags        = kNumberMask | kDoubleMask  | kNumberType,

    kNumberAnyFlags     = kNumberMask | kIntegerMask
                         | kInt32Mask | kUInt32Mask  | kInt64Mask | kUInt64Mask
                         | kFloatMask | kDoubleMask  | kNumberType,

    kConstStringFlags   = kStringMask                   | kStringType,   
    kCopyStringFlags    = kStringMask | kCopyStringMask | kStringType,
    kShortStringFlags   = kStringMask | kCopyStringMask | kInlineStringMask | kStringType,
    kStringFlags        = kConstStringFlags,

    kArrayMask          = kArrayType,
    kObjectMask         = kObjectType,
    kNullMask           = kNullType,

    kArrayFlags         = kArrayType,
    kObjectFlags        = kObjectType,
    kNullFlags          = kNullType,

    // Value type mask
    kTypeMask           = 0xFF,
    kFlagMask           = 0xFFFFFF00
};

/****************************************************************

// Forward declaration.
template <typename EncodingT = JSONFX_DEFAULT_ENCODING,
          typename PoolAllocatorT = DefaultPoolAllocator>
class BasicValue;

/****************************************************************/

//#include "JsonFx/Detail/Reader-Fwd.h"

// Forward declaration.
template <typename EncodingT = JSONFX_DEFAULT_ENCODING,
          typename PoolAllocatorT = DefaultPoolAllocator>
class BasicValue;

// Define default Value class type
typedef BasicValue<>    Value;

// Forward declaration.
template <typename EncodingT,
          typename PoolAllocatorT> 
class BasicMember;

template <bool IsConst, typename EncodingT, typename PoolAllocatorT>
class BasicMemberIterator;

// Save and setting the packing alignment
#pragma pack(push)
#pragma pack(1)

template <typename EncodingT /* = JSONFX_DEFAULT_ENCODING */,
          typename PoolAllocatorT /* = DefaultPoolAllocator */>
class BasicValue {
public:
    typedef typename EncodingT::CharType    CharType;
    typedef EncodingT                       EncodingType;
    typedef PoolAllocatorT                  PoolAllocatorType;
    typedef BasicStringRef<CharType>        StringRefType;      //!< Reference to a constant string

    typedef BasicMember<EncodingT, PoolAllocatorT>  MemberType;

    typedef BasicValue *                    ValueIterator;      //!< Value iterator for iterating in array.
    typedef const BasicValue *              ConstValueIterator; //!< Constant value iterator for iterating in array.

    //!< Member iterator for iterating in object.
    typedef typename BasicMemberIterator<false, EncodingT, PoolAllocatorT>::Iterator    MemberIterator;
    //!< Constant member iterator for iterating in object.
    typedef typename BasicMemberIterator<true,  EncodingT, PoolAllocatorT>::Iterator    ConstMemberIterator;

    typedef uint32_t                        SizeType;
    typedef uint32_t                        ValueType;

public:
    union Number {
        char        c;
        int8_t      i8;
        uint8_t     u8;
        int16_t     i16;
        uint16_t    u16;
        int32_t     i32;
        uint32_t    u32;
        int64_t     i64;
        uint64_t    u64;
        float       f;
        double      d;
    };

    struct String {
        SizeType         size;
        SizeType         capacity;
        unsigned int     hashCode;
        const CharType * data;
    };

    struct SmallString {
        SizeType         size;
        SizeType         capacity;
        unsigned int     hashCode;
        const CharType * data;

        SizeType GetLength() const { return size; }
    };

    struct Element {
        void * data;
    };

    struct Array {
        SizeType        size;
        SizeType        capacity;
        unsigned int    hashCode;
        BasicValue *    elements;
    };

    struct Object {
        SizeType        size;
        SizeType        capacity;
        unsigned int    hashCode;
        MemberType *    members;
    };

    union ValueData {
        String      str;
        SmallString sso;
        Number      num;
        Array       array;
        Object      obj;
    };

private:
    ValueType   mValueType;
    ValueData   mValueData;

public:
    BasicValue() : mValueType(kNullFlags), mValueData() {}

    BasicValue(const CharType * str) {
        mValueType = kStringFlags;
        mValueData.str.data = str;
        mValueData.str.size = ::strlen(str);
    }

    explicit BasicValue(StringRefType str) : mValueData(), mValueType() { setStringRaw(str); }

    ~BasicValue() { release(); }

private:
    //! Copy constructor is not permitted.
    BasicValue(const BasicValue & rhs);

public:
    void visit();

    void release() {
        //printf("JsonFx::BasicValue::release() enter.\n");
        // Shortcut by Allocator's trait
        if (PoolAllocatorType::kNeedFree) {
            switch (mValueType) {
            case kArrayFlags:
                for (BasicValue * v = mValueData.array.elements; v != mValueData.array.elements + mValueData.array.size; ++v) {
                    v->~BasicValue();
                }
                PoolAllocatorType::deallocate(mValueData.array.elements);
                break;

            case kObjectFlags:
                for (MemberIterator m = getMemberBegin(); m != getMemberEnd(); ++m) {
                    m->~MemberType();
                }
                PoolAllocatorType::deallocate(mValueData.obj.members);
                break;

            case kCopyStringFlags:
                PoolAllocatorType::deallocate(const_cast<CharType *>(mValueData.str.data));
                break;

            default:
                printf("JsonFx::BasicValue::release() -- switch(mValueType) branch = default, mValueType = 0x%08X\n", mValueType);
                break;  // Do nothing for other types.
            }
        }
        //printf("JsonFx::BasicValue::release() over.\n");
    }

    void setStringRaw(StringRefType str) {
        mValueType = kConstStringMask;
        mValueData.str.data = str.mData;
        mValueData.str.size = str.mSize;
    }

    void setObject() {
        mValueType = kObjectMask;
        mValueData.obj.members = NULL;
        mValueData.obj.size = 0;
        mValueData.obj.capacity = 0;
    }

    ValueType getType()  const { return static_cast<ValueType>(mValueType & kTypeMask); }
    ValueType getFlags() const { return static_cast<ValueType>(mValueType & kFlagMask); }

    bool isNull()   const { return (mValueType == kNullFlags);              }
    bool isFalse()  const { return (mValueType == kFalseFlags);             }
    bool isTrue()   const { return (mValueType == kTrueFlags);              }
    bool isBool()   const { return ((mValueType & kBoolFlags)   != 0);      }
    bool isObject() const { return (mValueType == kObjectFlags);            }
    bool isArray()  const { return (mValueType == kArrayFlags);             }
    bool isNumber() const { return ((mValueType & kNumberMask)  != 0);      }
    bool isInt()    const { return ((mValueType & kIntFlags)    != 0);      }
    bool isUint()   const { return ((mValueType & kUIntFlags)   != 0);      }
    bool isInt64()  const { return ((mValueType & kInt64Flags)  != 0);      }
    bool isUint64() const { return ((mValueType & kUInt64Flags) != 0);      }
    bool isFloat()  const { return ((mValueType & kFloatFlags)  != 0);      }
    bool isDouble() const { return ((mValueType & kDoubleFlags) != 0);      }
    bool isString() const { return ((mValueType & kStringFlags) != 0);      }

    void setNull() { mValueType = kNullFlags; }

    //MemberIterator findMember(const CharType * name) { return MemberIterator(NULL); }
    MemberIterator findMember(const CharType * name) {
        BasicValue n(StringRefType(name).mData);
        return findMember(n);
    }

    ConstMemberIterator findMember(const CharType * name) const {
        return const_cast<BasicValue &>(*this).findMember(name);
    }

    template <typename SourceAllocatorT>
    MemberIterator findMember(const BasicValue<EncodingT, SourceAllocatorT> & name) {
        jimi_assert(isObject());
        jimi_assert(name.isString());
        MemberIterator member = getMemberBegin();
        for ( ; member != getMemberEnd(); ++member)
            if (name.stringEqual(member->name)) {
                break;
            }
        return member;
    }

    template <typename SourceAllocatorT>
    ConstMemberIterator findMember(const BasicValue<EncodingT, SourceAllocatorT> & name) const {
        return const_cast<BasicValue &>(*this).findMember(name);
    }

    MemberIterator getMemberBegin() {
        jimi_assert(isObject());
        return MemberIterator(mValueData.obj.members);
    }
    MemberIterator getMemberEnd()   {
        jimi_assert(isObject());
        return MemberIterator(mValueData.obj.members + mValueData.obj.size);
    }

    ConstMemberIterator getMemberBegin() const {
        jimi_assert(isObject());
        return ConstMemberIterator(mValueData.obj.members);
    }
    ConstMemberIterator getMemberEnd() const   {
        jimi_assert(isObject());
        return ConstMemberIterator(mValueData.obj.members + mValueData.obj.size);
    }

    bool hasMember(const CharType * name) const {
        return (findMember(name) != getMemberEnd());
    }

    template <typename SourceAllocatorT>
    bool hasMember(const BasicValue<EncodingT, SourceAllocatorT> & name) const {
        return findMember(name) != getMemberEnd();
    }

    template <typename SourceAllocatorT>
    BasicValue & operator[] (const BasicValue<EncodingT, SourceAllocatorT> & name) {
        MemberIterator member = findMember(name);
        if (member != getMemberEnd()) {
            return member->value;
        }
        else {
            // See above note
            jimi_assert(false);
            static BasicValue nullValue;
            return NullValue;
        }
    }

    template <typename SourceAllocatorT>
    bool stringEqual(const BasicValue<EncodingT, SourceAllocatorT> & rhs) const {
        jimi_assert(isString());
        jimi_assert(rhs.isString());

        const SizeType len1 = getStringLength();
        const SizeType len2 = rhs.getStringLength();
        if (len1 != len2) { return false; }

        const CharType * const str1 = getString();
        const CharType * const str2 = rhs.getString();
        // fast path for constant string
        if (str1 == str2) { return true; }

        return (std::memcmp(str1, str2, sizeof(CharType) * len1) == 0);
    }

    const CharType * getString() const {
        jimi_assert(isString());
        return ((mValueType & kInlineStringMask) ? mValueData.sso.data : mValueData.str.data);
    }

    SizeType getStringLength() const {
        jimi_assert(isString());
        return ((mValueType & kInlineStringMask) ? (mValueData.sso.GetLength()) : mValueData.str.size);
    }
};

// Recover the packing alignment
#pragma pack(pop)

template <typename EncodingT, typename PoolAllocatorT>
void BasicValue<EncodingT, PoolAllocatorT>::visit()
{
    printf("JsonFx::BasicValue::visit(). EncodingType = %d\n\n", EncodingType::type);
}

}  // namespace JsonFx

// Define default Value class type
typedef JsonFx::BasicValue<>     jfxValue;

#endif  /* !_JSONFX_VALUE_H_ */
