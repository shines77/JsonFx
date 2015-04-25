
#ifndef _JSONFX_IOSTREAM_IOSTREAMROOT_H_
#define _JSONFX_IOSTREAM_IOSTREAMROOT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "jimi/basic/stdsize.h"

#include "JsonFx/Config.h"
#include "JsonFx/Internal/Closeable.h"

namespace JsonFx {

enum ios_state {
    kIOsNoneMask    = 0,
    kIOsGoodMask    = 1,
    kIOsEofMask     = 2,

    kIOsFullMask    = 0xFFFFFFFF
};

// Forward declaration.
template <typename T = JSONFX_DEFAULT_CHARTYPE>
class BasicIOStreamRoot;

// Define default BasicIOStreamRoot<T>.
typedef BasicIOStreamRoot<>  IOStreamRoot;

template <typename T>
class BasicIOStreamRoot : public internal::Closeable<T>
{
public:
    typedef T           CharType;
    typedef size_t      SizeType;
    typedef uint32_t    MaskType;

public:
    // Whether support mark() method?
    static const bool kSupportMarked = false;

private:
    CharType *  mCurrent;
    CharType *  mBegin;
    CharType *  mEnd;
    SizeType    mSize;
    MaskType    mState;
    SizeType    mAvailable;
    CharType *  mMark;

public:
    BasicIOStreamRoot() :
        mCurrent(NULL), mBegin(NULL), mEnd(NULL), mSize(0), mState(0), mAvailable(0), mMark(NULL)
    {
        jfx_iostream_trace("00 BasicIOStreamRoot<T>::BasicIOStreamRoot() visited.\n");
    }

    BasicIOStreamRoot(void * buffer, SizeType size) :
        mCurrent(buffer), mBegin(buffer), mEnd(mBegin + size * sizeof(CharType)),
        mSize(size), mState(0), mAvailable(0), mMark(NULL)
    {
        jfx_iostream_trace("00 BasicIOStreamRoot<T>::BasicIOStreamRoot() visited.\n");
    }

    ~BasicIOStreamRoot() {
        jfx_iostream_trace("01 BasicIOStreamRoot<T>::~BasicIOStreamRoot() visited.\n");
        close();
    }

    bool isValid() { return (mCurrent != NULL);      }
    bool isGood()  { return (mState & kIOsGoodMask); }
    bool isEOF() {
        jimi_assert(mCurrent != NULL);
        jimi_assert(mCurrent <= mEnd);
        return (mCurrent != mEnd);
    }

    bool isTopOverflow()                     { return (mCurrent   < mBegin); }
    bool isBottomOverflow()                  { return (mCurrent   > mEnd);   }
    bool isTopOverflow(void * newCurrent)    { return (newCurrent < mBegin); }
    bool isBottomOverflow(void * newCurrent) { return (newCurrent > mEnd);   }

    void * getCurrent()  { return = reinterpret_cast<void *>(mCurrent); }
    char * getCurrentN() { return = reinterpret_cast<char *>(mCurrent); }

    void * getBegin()    { return = reinterpret_cast<void *>(mBegin); }
    void * getEnd()      { return = reinterpret_cast<void *>(mEnd);   }

    SizeType getBottomDistance() {
#if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) \
        || defined(__amd64__) || defined(__x86_64__)
        return reinterpret_cast<SizeType>(reinterpret_cast<char *>(0xFFFFFFFFFFFFFFFFULL) - getCurrentT());
#else
        return reinterpret_cast<SizeType>(reinterpret_cast<char *>(0xFFFFFFFFUL) - getCurrentT());
#endif
    }

    void close() {
        jfx_iostream_trace("10 BasicIOStreamRoot<T>::close() visited.\n");
    }

    void flush() {
        jfx_iostream_trace("10 BasicIOStreamRoot<T>::flush() visited.\n");
    }

    int available() {
        jfx_iostream_trace("10 BasicIOStreamRoot<T>::available() visited.\n");
        return -1;
    }

    void reset() { mCurrent = mBegin; }
    
    bool markSupported() { return kSupportMarked; }
    void mark(int readlimit) {
        jimi_assert(this->markSupported());
        mMark = mCurrent;
    }
    void resetMark() {
        jimi_assert(this->markSupported());
        if (this->markSupported())
            mCurrent = mMark;
    }

    void * seek(SizeType pos) {
        void * newCurrent = reinterpret_cast<void *>(reinterpret_cast<char *>(mBegin) + pos);
        checkBottomAndUpdate(newCurrent);
        return newCurrent;
    }

    void * skipTo(int n) {
        void * newCurrent = reinterpret_cast<void *>(reinterpret_cast<char *>(mCurrent) + n);
        return newCurrent;
    }

    void * moveCursor(int offset) {
        void * newCurrent = reinterpret_cast<void *>(reinterpret_cast<char *>(mCurrent) + offset);
        checkEdgeAndUpdate(newCurrent);
        return newCurrent;
    }

protected:
    void * checkTopAndUpdate(void * newCurrent) {
        if (newCurrent < mBegin)
            newCurrent = mBegin;
        mCurrent = newCurrent;
        return newCurrent;
    }

    void * checkBottomAndUpdate(void * newCurrent) {
        if (newCurrent > mEnd)
            newCurrent = mEnd;
        mCurrent = newCurrent;
        return newCurrent;
    }

    void * checkEdgeAndUpdate(void * newCurrent) {
        if (newCurrent > mEnd)
            newCurrent = mEnd;
        else if (newCurrent < mBegin)
            newCurrent = mBegin;
        mCurrent = newCurrent;
        return newCurrent;
    }
};

}  // namespace JsonFx

// Define default IOStreamRoot class type
typedef JsonFx::BasicIOStreamRoot<JSONFX_DEFAULT_CHARTYPE>    jfxIOStreamRoot;

#endif  /* _JSONFX_IOSTREAM_IOSTREAMROOT_H_ */
