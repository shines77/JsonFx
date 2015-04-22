
#ifndef _JSONFX_STREAM_INPUTSTREAM_H_
#define _JSONFX_STREAM_INPUTSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "jimi/basic/stdsize.h"
#include "JsonFx/Internal/Readable.h"
#include "JsonFx/Stream/IOStreamRoot.h"

namespace JsonFx {

// Forward declaration.
template <typename T>
class BasicInputStream;

// Define default BasicInputStream<T>.
typedef BasicInputStream<_Char>  InputStream;

template <typename T>
class BasicInputStream : virtual public BasicIOStreamRoot<T>,
                         public internal::Readable<T>
{
public:
    typedef typename BasicIOStreamRoot<T>::CharType  CharType;
    typedef typename BasicIOStreamRoot<T>::SizeType  SizeType;

public:
    // Whether support mark() method?
    static const bool kSupportMarked = false;

public:
    BasicInputStream() {
        printf("00 BasicInputStream<T>::BasicInputStream() visited.\n");
    }

    ~BasicInputStream() {
        printf("00 BasicInputStream<T>::~BasicInputStream() visited.\n");
        close();
    }

    void close() {
        printf("10 BasicInputStream<T>::close() visited.\n");
#if 0
        T * pThis = static_cast<T *>(this);
        if (pThis != NULL)
            pThis->close();
#endif
    };

    int available() {
        printf("10 BasicInputStream<T>::available() visited.\n");
#if 0
        T * pThis = static_cast<T *>(this);
        if (pThis != NULL)
            return pThis->available();
        else
            return 0;
#endif
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

#endif  /* _JSONFX_STREAM_INPUTSTREAM_H_ */
