
#ifndef _JSONFX_STREAM_OUTPUTSTREAM_H_
#define _JSONFX_STREAM_OUTPUTSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "jimi/basic/stdsize.h"
#include "JsonFx/Stream/IOStream.h"

namespace JsonFx {

// Forward declaration.
template <typename T>
class OutputStreamBase;

// Define default OutputStreamBase<T>
typedef OutputStreamBase<_Char>  OutputStream;

template <typename T>
class OutputStreamBase : public IOStreamBase<T>
{
public:
    typedef typename IOStreamBase<T>::CharType    CharType;
    typedef typename IOStreamBase<T>::SizeType    SizeType;

public:
    // Whether support mark() method?
    static const bool kSupportMarked = false;

public:
    OutputStreamBase() {
        printf("00 OutputStreamBase<T>::OutputStreamBase() visited.\n");
    }

    ~OutputStreamBase() {
        printf("00 OutputStreamBase<T>::~OutputStreamBase() visited.\n");
        close();
    }

    void close() {
        printf("10 OutputStreamBase<T>::close() visited.\n");
#if 0
        T * pThis = static_cast<T *>(this);
        if (pThis != NULL)
            pThis->close();
#endif
    };

    int available() {
        printf("10 OutputStreamBase<T>::available() visited.\n");
#if 0
        T * pThis = static_cast<T *>(this);
        if (pThis != NULL)
            return pThis->available();
        else
            return 0;
#endif
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

#endif  /* _JSONFX_STREAM_OUTPUTSTREAM_H_ */
