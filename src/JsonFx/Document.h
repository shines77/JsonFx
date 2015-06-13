
#ifndef _JSONFX_DOCUMENT_H_
#define _JSONFX_DOCUMENT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "JsonFx/Config.h"
#include "JsonFx/Allocator.h"
#include "JsonFx/Value.h"
#include "JsonFx/Reader.h"
#include "JsonFx/Writer.h"

#include "JsonFx/Stream/StringInputStream.h"

#include "jimi/basic/assert.h"

namespace JsonFx {

// Forward declaration.
template <typename EncodingT = JSONFX_DEFAULT_ENCODING,
          typename PoolAllocatorT = DefaultPoolAllocator,
          typename AllocatorT = DefaultAllocator>
class BasicDocument;

// Define default Document class type
typedef BasicDocument<>    Document;

// Save and setting the packing alignment
#pragma pack(push)
#pragma pack(1)

template <typename EncodingT /* = JSONFX_DEFAULT_ENCODING */,
          typename PoolAllocatorT /* = DefaultPoolAllocator */,
          typename AllocatorT /* = DefaultAllocator */>
class BasicDocument : public BasicValue<EncodingT, PoolAllocatorT>
{
public:
    typedef typename EncodingT::CharType            CharType;           //!< Character type derived from Encoding.
    typedef BasicValue<EncodingT, PoolAllocatorT>   ValueType;          //!< Value type of the document.
    typedef EncodingT                               EncodingType;       //!< Character encoding type.
    typedef PoolAllocatorT                          PoolAllocatorType;  //!< Pool allocator type from template parameter.
    typedef AllocatorT                              StackAllocatorType; //!< Stack allocator type from template parameter.
    typedef BasicStringInputStream<CharType>        StringInputStreamType;
    typedef BasicStack<PoolAllocatorT>              StackType;
    typedef BasicParseResult<EncodingT>             ParseResultType;

private:
    PoolAllocatorType *     mPoolAllocator;
    bool                    mPoolAllocatorNeedFree;
    StackAllocatorType *    mStackAllocator;
    bool                    mStackAllocatorNeedFree;
    StackType               mStack;
    ParseResultType         mParseResult;

public:
    BasicDocument(const PoolAllocatorType * poolAllocator = NULL)
        : mPoolAllocator(const_cast<PoolAllocatorType *>(poolAllocator)),
          mPoolAllocatorNeedFree(false),
          mStack(poolAllocator, 100), mParseResult()
    {
        initPoolAllocator(poolAllocator);
    }

    ~BasicDocument() {
        destroy();
    }

private:
    //! Prohibit copying
    BasicDocument(const BasicDocument &);
    //! Prohibit assignment
    BasicDocument & operator =(const BasicDocument &);

public:
    const PoolAllocatorType *  getPoolAllocator() const  { return mPoolAllocator;  }
    const StackAllocatorType * getStackAllocator() const { return mStackAllocator; }
    const PoolAllocatorType *  getAllocator() const      { return getPoolAllocator();  }

    void visit();

    void test() {
        printf("JsonFx::BasicDocument::test()\n\n");
    }

private:
    void destroy() {
        if (this->mPoolAllocatorNeedFree) {
            if (this->mPoolAllocator) {
                delete this->mPoolAllocator;
                this->mPoolAllocator = NULL;
            }
        }
    }

    void initPoolAllocator(const PoolAllocatorType * poolAllocator) {
        if (poolAllocator == NULL) {
            PoolAllocatorType * newPoolAllocator = new PoolAllocatorType();
            jimi_assert(newPoolAllocator != NULL);
            if (newPoolAllocator != NULL) {
                mPoolAllocator = newPoolAllocator;
                mPoolAllocatorNeedFree = true;
            }
        }
    }

public:
    // Implementation of ReaderHandler
    bool saxNull()             { new (mStack.template push<ValueType>()) ValueType();  return true; }
    bool saxBool(bool b)       { new (mStack.template push<ValueType>()) ValueType(b); return true; }
    bool saxInt(int i)         { new (mStack.template push<ValueType>()) ValueType(i); return true; }
    bool saxUint(unsigned i)   { new (mStack.template push<ValueType>()) ValueType(i); return true; }
    bool saxInt64(int64_t i)   { new (mStack.template push<ValueType>()) ValueType(i); return true; }
    bool saxUint64(uint64_t i) { new (mStack.template push<ValueType>()) ValueType(i); return true; }
    bool saxDouble(double d)   { new (mStack.template push<ValueType>()) ValueType(d); return true; }

    bool saxString(const CharType * str, SizeType length, bool copy) { 
        if (copy) 
            new (mStack.template push<ValueType>()) ValueType(str, length, this->getAllocator());
        else
            new (mStack.template push<ValueType>()) ValueType(str, length);
        return true;
    }

    bool saxStartObject() { new (mStack.template push<ValueType>()) ValueType(kObjectType); return true; }
    
    bool saxKey(const CharType * str, SizeType length, bool copy) { return saxString(str, length, copy); }

    bool saxEndObject(SizeType memberCount) {
        typename ValueType::MemberType * members = mStack.template pop<typename ValueType::MemberType>(memberCount);
        mStack.template Top<ValueType>()->setObjectRaw(members, (SizeType)memberCount, this->getAllocator());
        return true;
    }

    bool saxStartArray() { new (mStack.template push<ValueType>()) ValueType(kArrayType); return true; }
    
    bool saxEndArray(SizeType elementCount) {
        ValueType * elements = mStack.template pop<ValueType>(elementCount);
        mStack.template Top<ValueType>()->setArrayRaw(elements, elementCount, this->getAllocator());
        return true;
    }
    // End of implementation of ReaderHandler

    //
    // BasicDocument::parse<parseFlags, SourceEncodingT, InuptStreamT>(is);
    //
    template <size_t parseFlags, typename SourceEncodingT, typename InuptStreamT>
    BasicDocument & parseStream(const InuptStreamT & is) {
        // Remove existing root if exist
        ValueType::setNull();
        BasicReader<parseFlags, SourceEncodingT, EncodingT, PoolAllocatorT, AllocatorT>
            reader(this->getPoolAllocator(), false, this->getPoolAllocator());

        // Parse the stream use SAX mode in Reader class.
        mParseResult = reader.parseStream(const_cast<InuptStreamT &>(is), *this);
        if (mParseResult.hasError()) {
            // Get the error code.
            mParseResult.getError();
        }
        return *this;
    }

    //
    // BasicDocument::parse(InputStreamT & is);
    //
    template <size_t parseFlags, typename InputStreamT>
    BasicDocument & parseStream(const InputStreamT & is) {
        return parseStream<parseFlags, EncodingT, InputStreamT>(is);
    }

    template <typename InputStreamT>
    BasicDocument & parseStream(const InputStreamT & is) {
        return parseStream<kDefaultParseFlags>(is);
    }

    //
    // BasicDocument::parse(StringInputStreamType & is);
    //
    template <size_t parseFlags, typename SourceEncodingT>
    BasicDocument & parseStream(const StringInputStreamType & is) {
        return parseStream<parseFlags, SourceEncodingT, StringInputStreamType>(is);
    }

    template <size_t parseFlags>
    BasicDocument & parseStream(const StringInputStreamType & is) {
        return parseStream<parseFlags, EncodingT>(is);
    }

    BasicDocument & parseStream(const StringInputStreamType & is) {
        return parseStream<kDefaultParseFlags>(is);
    }

    //
    // BasicDocument::parse(const CharType * text);
    //
    template <size_t parseFlags, typename SourceEncodingT, typename InputStreamT>
    BasicDocument & parseStream(const CharType * text) {
        jimi_assert(text != NULL);
        InputStreamT inputStream(text);
        return parseStream<parseFlags, SourceEncodingT, InputStreamT>(inputStream);
    }

    template <size_t parseFlags, typename SourceEncodingT>
    BasicDocument & parseStream(const CharType * text) {
        return parseStream<parseFlags, SourceEncodingT, StringInputStreamType>(text);
    }

    template <typename InputStreamT>
    BasicDocument & parseStream(const typename InputStreamT::CharType * text) {
        return parseStream<kDefaultParseFlags, EncodingT, InputStreamT>(text);
    }

    template <size_t parseFlags>
    BasicDocument & parseStream(const CharType * text) {
        return parseStream<parseFlags, EncodingT>(text);
    }

    BasicDocument & parseStream(const CharType * text) {
        return parseStream<kDefaultParseFlags>(text);
    }

    //
    // BasicDocument::parse<parseFlags, SourceEncodingT, InuptStreamT>(is);
    //
    template <size_t parseFlags, typename SourceEncodingT, typename InuptStreamT>
    BasicDocument & parse(const InuptStreamT & is) {
        // Remove existing root if exist
        ValueType::setNull();
        BasicReader<parseFlags, SourceEncodingT, EncodingT, PoolAllocatorT, AllocatorT>
            reader(this->getPoolAllocator(), false, this->getPoolAllocator());

        // Parse the stream use SAX mode in Reader class.
        mParseResult = reader.parse(const_cast<InuptStreamT &>(is), *this);
        if (mParseResult.hasError()) {
            // Get the error code.
            mParseResult.getError();
        }
        return *this;
    }

    //
    // BasicDocument::parse(InputStreamT & is);
    //
    template <size_t parseFlags, typename InputStreamT>
    BasicDocument & parse(const InputStreamT & is) {
        return parse<parseFlags, EncodingT, InputStreamT>(is);
    }

    template <typename InputStreamT>
    BasicDocument & parse(const InputStreamT & is) {
        return parse<kDefaultParseFlags>(is);
    }

    //
    // BasicDocument::parse(StringInputStreamType & is);
    //
    template <size_t parseFlags, typename SourceEncodingT>
    BasicDocument & parse(const StringInputStreamType & is) {
        return parse<parseFlags, SourceEncodingT, StringInputStreamType>(is);
    }

    template <size_t parseFlags>
    BasicDocument & parse(const StringInputStreamType & is) {
        return parse<parseFlags, EncodingT>(is);
    }

    BasicDocument & parse(const StringInputStreamType & is) {
        return parse<kDefaultParseFlags>(is);
    }

    //
    // BasicDocument::parse(const CharType * text);
    //
    template <size_t parseFlags, typename SourceEncodingT, typename InputStreamT>
    BasicDocument & parse(const CharType * text) {
        jimi_assert(text != NULL);
        InputStreamT inputStream(text);
        return parse<parseFlags, SourceEncodingT, InputStreamT>(inputStream);
    }

    template <size_t parseFlags, typename SourceEncodingT>
    BasicDocument & parse(const CharType * text) {
        return parse<parseFlags, SourceEncodingT, StringInputStreamType>(text);
    }

    template <typename InputStreamT>
    BasicDocument & parse(const typename InputStreamT::CharType * text) {
        return parse<kDefaultParseFlags, EncodingT, InputStreamT>(text);
    }

    template <size_t parseFlags>
    BasicDocument & parse(const CharType * text) {
        return parse<parseFlags, EncodingT>(text);
    }

    BasicDocument & parse(const CharType * text) {
        return parse<kDefaultParseFlags>(text);
    }
};

// Recover the packing alignment
#pragma pack(pop)

template <typename EncodingT, typename PoolAllocatorT, typename AllocatorT>
void BasicDocument<EncodingT, PoolAllocatorT, AllocatorT>::visit()
{
    printf("JsonFx::BasicDocument::visit(). EncodingType = %d\n\n", EncodingType::type);
}

}  // namespace JsonFx

// Define default Document class type
typedef JsonFx::BasicDocument<>     jfxDocument;

#endif  /* !_JSONFX_DOCUMENT_H_ */
