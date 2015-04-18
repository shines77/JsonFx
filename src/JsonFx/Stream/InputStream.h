
#ifndef _JSONFX_INPUTSTREAM_H_
#define _JSONFX_INPUTSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "jimi/basic/stdsize.h"

#include "JsonFx/Internal/Closeable.h"

namespace JsonFx {

class InputStream : public internal::Closeable
{
public:
    InputStream()  {
        printf("00 InputStream::InputStream() visited.\n");
    };

    virtual ~InputStream() {
        printf("01 InputStream::~InputStream() visited.\n");
    }

    virtual void close() = 0;
    virtual int  available() = 0;
    
    virtual void reset() = 0;
    virtual size_t skip(size_t n) = 0;

    virtual void mark(int readlimit) = 0;
    virtual bool markSupported() = 0;    

    /**
     * Reads the next byte of data from the input stream. The value byte is returned as an int in the range 0 to 255.
     * If no byte is available because the end of the stream has been reached, the value -1 is returned.
     * This method blocks until input data is available, the end of the stream is detected, or an exception is thrown.
     */
    virtual int read() = 0;
    virtual int read(void *buffer, int size) = 0;
    virtual int read(void *buffer, int size, int offset, int len) = 0;
};

template <typename T>
class InputStreamImpl : public internal::CloseableImpl
{
public:
    static const bool kSupportMarded = false;

public:
    InputStreamImpl() {
        printf("00 InputStreamImpl::InputStreamImpl() visited.\n");
    }

    ~InputStreamImpl() {
        printf("00 InputStreamImpl::~InputStreamImpl() visited.\n");
        close();
    }

    void close() {
        printf("10 InputStreamImpl::close() visited.\n");
        T * pThis = static_cast<T *>(this);
        if (pThis != NULL)
            pThis->close_impl();
    };

    int available() {
        printf("10 InputStreamImpl::available() visited.\n");
        T * pThis = static_cast<T *>(this);
        if (pThis != NULL)
            return pThis->available_impl();
        else
            return 0;
    }
    
    void reset() {}
    size_t skip(size_t n) { return 0; }

    void mark(int readlimit) {}
    bool markSupported() { return kSupportMarded; }

    /**
     * Reads the next byte of data from the input stream. The value byte is returned as an int in the range 0 to 255.
     * If no byte is available because the end of the stream has been reached, the value -1 is returned.
     * This method blocks until input data is available, the end of the stream is detected, or an exception is thrown.
     */
    int read() { return 0; }
    int read(void * buffer, int size) { return 0; }
    int read(void * buffer, int size, int offset, int len) { return 0; }

    void close_impl() {
        printf("20 InputStreamImpl::closeImpl() visited.\n");
    }

    int available_impl() {
        printf("20 InputStreamImpl::available_impl() visited.\n");
    }
};

}  // namespace JsonFx

#endif  /* _JSONFX_INPUTSTREAM_H_ */
