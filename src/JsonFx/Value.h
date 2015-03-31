
#ifndef _JSONFX_VALUE_H_
#define _JSONFX_VALUE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "jimi/basic/stdint.h"
#include "jimi/basic/stdsize.h"

#include "JsonFx/Config.h"
#include "JsonFx/CharSet.h"

namespace JsonFx {

enum ValueType {
    // Base value type
    kTypeNumber,
    kTypeString,
    kTypeTrue,
    kTypeFalse,
    kTypeArray,
    kTypeObject,
    kTypeNull,

    // Last value type
    kMaxValueType = kTypeNull + 1,

    // Extend value type
    kTypeBool = kMaxValueType,
    kTypeInteger,
    kTypeDouble,
    kTypeFloat,

    kMaxValueTypeEx,
};

enum ValueTypeMask {
    kBoolMask       = 0x000100,
    kInt8Mask       = 0x000200,
    kUInt8Mask      = 0x000400,
    kInt16Mask      = 0x000800,
    kUInt16Mask     = 0x001000,
    kInt32Mask      = 0x002000,
    kUInt32Mask     = 0x004000,
    kInt64Mask      = 0x008000,
    kUInt64Mask     = 0x010000,

    kIntegerMask    = 0x020000,
    kFloatMask      = 0x040000,
    kDoubleMask     = 0x080000,
    kNumberMask     = 0x100000,

    kStringMask     = 0x200000,
    kCopyMask       = 0x400000,
    kInlineMask     = 0x800000,

    kNumberBoolMaskBase = kNumberMask | kIntegerMask | kBoolMask,

    kNumberBoolMask     = kNumberMask | kIntegerMask | kBoolMask | kTypeBool,
    kTrueMask           = kNumberMask | kIntegerMask | kBoolMask | kTypeTrue,
    kFalseMask          = kNumberMask | kIntegerMask | kBoolMask | kTypeFalse,

    kNumberIntegerMaskBase = kNumberMask | kIntegerMask,

    kNumberIntegerMask  = kNumberMask | kIntegerMask | kTypeInteger,
    kNumberIntMask      = kNumberMask | kIntegerMask | kInt32Mask  | kTypeNumber,
    kNumberUIntMask     = kNumberMask | kIntegerMask | kUInt32Mask | kTypeNumber,

    kNumberInt64Mask    = kNumberMask | kIntegerMask | kInt64Mask  | kTypeNumber,
    kNumberUInt64Mask   = kNumberMask | kIntegerMask | kUInt64Mask | kTypeNumber,

    kNumberFloatMask    = kNumberMask | kFloatMask  | kTypeNumber,
    kNumberDoubleMask   = kNumberMask | kDoubleMask | kTypeNumber,

    kNumberAnyMask      = kNumberMask | kIntegerMask
                         | kInt32Mask | kUInt32Mask | kInt64Mask | kUInt64Mask
                         | kFloatMask | kDoubleMask | kTypeNumber,

    kConstStringMask    = kStringMask | kTypeString,
    kCopyStringMask     = kStringMask | kCopyMask | kTypeString,
    kShortStringMask    = kStringMask | kCopyMask | kInlineMask | kTypeString,

    kArrayMask      = kTypeArray,
    kObjectMask     = kTypeObject,
    kNullMask       = kTypeNull,

    // Value type mask
    kTypeMask       = 0xFF
};

// Save and setting the packing alignment
#pragma pack(push)
#pragma pack(1)

// Forward declaration.
template <typename EncodingType>
class BaseValue;

template <typename EncodingType = JSONFX_DEFAULT_ENCODING>
class BaseValue {
public:
    typedef typename EncodingType::CharType     CharType;

    typedef size_t                              SizeType;
    typedef uint32_t                            ValueType;

public:
    BaseValue()  {}
    ~BaseValue() {}

    void visit();

public:
    union Number {
        int64_t  i64;
        uint64_t u64;
        float    f;
        double   d;
    };

    struct String {
        CharType *      data;
        SizeType        size;
        SizeType        capacity;
        unsigned int    hashCode;
    };

    struct Element {
        void * data;
    };

    struct Array {
        Element *       items;
        SizeType        size;
        SizeType        capacity;
        unsigned int    hashCode;
    };

    struct Member {
        void * data;
    };

    struct Object {
        Member *        members;
        SizeType        size;
        SizeType        capacity;
        unsigned int    hashCode;
    };

    union ValueData
    {
        String  str;
        Number  num;
        Array   array;
        Object  obj;
    };

private:
    ValueType   mValueType;
    ValueData   mValueData;
};

// Recover the packing alignment
#pragma pack(pop)

// Define default Value class type
typedef BaseValue<>   Value;

template <typename CharType>
void BaseValue<CharType>::visit()
{
    printf("JsonFx::Value::visit() visited.\n\n");
}

}  // namespace JsonFx

#endif  /* !_JSONFX_VALUE_H_ */
