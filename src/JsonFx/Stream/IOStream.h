
#ifndef _JSONFX_STREAM_IOSTREAM_H_
#define _JSONFX_STREAM_IOSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "jimi/basic/stdsize.h"

#include "JsonFx/Config.h"
#include "JsonFx/Internal/Closeable.h"

namespace JsonFx {

// Forward declaration.
template <typename T>
class IOStreamBase;

// Define default IOStreamBase<T>.
typedef IOStreamBase<_Char>  IOStream;

template <typename T>
class IOStreamBase : public internal::Closeable
{
public:
    typedef T       CharType;
    typedef size_t  SizeType;

public:
    // Whether support mark() method?
    static const bool kSupportMarked = false;

public:
    IOStreamBase() {
        printf("00 IOStreamBase<T>::IOStreamBase() visited.\n");
    }

    ~IOStreamBase() {
        printf("00 IOStreamBase<T>::~IOStreamBase() visited.\n");
        close();
    }

    void close() {
        printf("10 IOStreamBase<T>::close() visited.\n");
    };

    int available() {
        printf("10 IOStreamBase<T>::available() visited.\n");
        return 0;
    }
    
    void reset() {}
    size_t skip(size_t n) { return 0; }

    void mark(int readlimit) {}
    bool markSupported() { return kSupportMarked; }

    /**
     * Reads the next byte of data from the input stream. The value byte is returned as an int in the range 0 to 255.
     * If no byte is available because the end of the stream has been reached, the value -1 is returned.
     * This method blocks until input data is available, the end of the stream is detected, or an exception is thrown.
     */
    int read() { return 0; }
    int read(void * buffer, int size) { return 0; }
    int read(void * buffer, int size, int offset, int len) { return 0; }
};

}  // namespace JsonFx

#endif  /* _JSONFX_STREAM_IOSTREAM_H_ */
