
#ifndef _JSONFX_IOSTREAM_INPUTIOSTREAM_H_
#define _JSONFX_IOSTREAM_INPUTIOSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "jimi/basic/stdsize.h"
#include "JsonFx/Internal/Readable.h"
#include "JsonFx/IOStream/IOStreamRoot.h"

namespace JsonFx {

// Forward declaration.
template <typename T = JSONFX_DEFAULT_CHARTYPE>
class BasicInputIOStream;

// Define default BasicInputIOStream<T>.
typedef BasicInputIOStream<>    InputIOStream;

template <typename T>
class BasicInputIOStream : virtual public BasicIOStreamRoot<T>,
                                   public internal::Readable<T>
{
public:
    typedef typename BasicIOStreamRoot<T>::CharType  CharType;
    typedef typename BasicIOStreamRoot<T>::SizeType  SizeType;

public:
    // Whether support mark() method?
    static const bool kSupportMarked = false;

public:
    BasicInputIOStream() {
        jfx_iostream_trace("00 BasicInputIOStream<T>::BasicInputIOStream() visited.\n");
    }

    ~BasicInputIOStream() {
        jfx_iostream_trace("01 BasicInputIOStream<T>::~BasicInputIOStream() visited.\n");
        close();
    }

    void close() {
        jfx_iostream_trace("10 BasicInputIOStream<T>::close() visited.\n");
#if 0
        T * pThis = static_cast<T *>(this);
        if (pThis != NULL)
            pThis->close();
#endif
    };

    int available() {
        jfx_iostream_trace("10 BasicInputIOStream<T>::available() visited.\n");
#if 0
        T * pThis = static_cast<T *>(this);
        if (pThis != NULL)
            return pThis->available();
        else
            return 0;
#endif
    }
    
    bool markSupported() { return kSupportMarked; }

    CharType get()  { retuen *mCurrent; }
    CharType peek() { retuen *mCurrent; }

    CharType take() { retuen *mCurrent++; }

    /**
     * Reads the next byte of data from the input stream. The value byte is returned as an int in the range 0 to 255.
     * If no byte is available because the end of the stream has been reached, the value -1 is returned.
     * This method blocks until input data is available, the end of the stream is detected, or an exception is thrown.
     */
#if 1
    int read() {
        jimi_assert(mCurrent != NULL)
        return (mCurrent != mEnd) ? reinterpret_cast<int>(*mCurrent) : reinterpret_cast<int>(-1);
    }
#else
    int read() {
        jimi_assert(mCurrent != NULL)
        if (mCurrent != mEnd)
            return reinterpret_cast<int>(*mCurrent)
        else
            return reinterpret_cast<int>(-1);
    }
#endif

    int read(CharType & c) {
        c = *mCurrent;
        if (mCurrent != mEnd)
            return reinterpret_cast<int>(*mCurrent)
        else
            return reinterpret_cast<int>(-1);
    }

    int read(void * buffer, int size) {
        return 0;
    }
    int read(void * buffer, int size, int offset, int len) { return 0; }
};

}  // namespace JsonFx

// Define default InputStream class type
typedef JsonFx::BasicInputIOStream<JSONFX_DEFAULT_CHARTYPE>    jfxInputIOStream;

#endif  /* _JSONFX_IOSTREAM_INPUTIOSTREAM_H_ */
