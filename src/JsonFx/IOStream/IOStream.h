
#ifndef _JSONFX_IOSTREAM_IOSTREAM_H_
#define _JSONFX_IOSTREAM_IOSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "jimi/basic/stdsize.h"

#include "JsonFx/IOStream/InputIOStream.h"
#include "JsonFx/IOStream/OutputIOStream.h"

namespace JsonFx {

// Forward declaration.
template <typename T = JSONFX_DEFAULT_CHARTYPE>
class BasicIOStream;

// Define default BasicIOStream<T>.
typedef BasicIOStream<>  IOStream;

template <typename T>
class BasicIOStream : public BasicInputIOStream<T>,
                      public BasicOutputIOStream<T>
{
public:
    typedef typename BasicInputIOStream<T>::CharType    CharType;
    typedef typename BasicInputIOStream<T>::SizeType    SizeType;

public:
    // Whether support mark() method?
    static const bool kSupportMarked = false;

public:
    BasicIOStream() {
        jfx_iostream_trace("00 BasicIOStream<T>::BasicIOStream() visited.\n");
    }

    ~BasicIOStream() {
        jfx_iostream_trace("00 BasicIOStream<T>::~BasicIOStream() visited.\n");
        close();
    }

    void close() {
        jfx_iostream_trace("10 BasicIOStream<T>::close() visited.\n");
    };

    int available() {
        jfx_iostream_trace("10 BasicIOStream<T>::available() visited.\n");
        return 0;
    }
    
    bool markSupported() { return kSupportMarked; }
    void mark(int readlimit) {}
    void reset() {}

    size_t skip(size_t n) { return 0; }

    /**
     * Reads the next byte of data from the input stream. The value byte is returned as an int in the range 0 to 255.
     * If no byte is available because the end of the stream has been reached, the value -1 is returned.
     * This method blocks until input data is available, the end of the stream is detected, or an exception is thrown.
     */
    int read() { return 1; }
    int read(CharType & c) { return 1; }
    int read(void * buffer, int size) { return 0; }
    int read(void * buffer, int size, int offset, int len) { return 0; }
};

}  // namespace JsonFx

// Define default IOStream class type
typedef JsonFx::BasicIOStream<JSONFX_DEFAULT_CHARTYPE>    jfxIOStream;

#endif  /* _JSONFX_IOSTREAM_IOSTREAM_H_ */
