
#ifndef _JSONFX_STREAM_FILE_OUTPUTSTREAM_H_
#define _JSONFX_STREAM_FILE_OUTPUTSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>
#include <string>

#include "jimi/basic/stdsize.h"
#include "jimi/basic/assert.h"

#include "JsonFx/Stream/Detail/FileDef.h"
#include "JsonFx/Stream/OutputStream.h"

namespace JsonFx {

// Forward declaration.
template <typename T>
class BasicFileOutputStream;

// Define default BasicFileOutputStream<T>.
typedef BasicFileOutputStream<_Char>  FileOutputStream;

template <typename T>
class BasicFileOutputStream : public BasicOutputStream<T>
{
public:
    typedef typename BasicOutputStream<T>::CharType    CharType;
    typedef typename BasicOutputStream<T>::SizeType    SizeType;

public:
    static const bool kSupportMarked = true;

private:
    FILE *  mFile;

public:
    BasicFileOutputStream() : mFile(NULL) {
        printf("00 BasicFileOutputStream<T>::BasicFileOutputStream() visited.\n");
    }

    BasicFileOutputStream(FILE * hFile) : mFile(hFile) {
        printf("00 BasicFileOutputStream<T>::BasicFileOutputStream(FILE * hFile) visited.\n");
    }

    BasicFileOutputStream(char * filename) : mFile(NULL) {
        printf("00 BasicFileOutputStream<T>::BasicFileOutputStream(char * filename) visited.\n");
        mFile = fopen(filename, "rb");
    }

    BasicFileOutputStream(std::string filename) : mFile(NULL) {
        printf("00 BasicFileOutputStream<T>::BasicFileOutputStream(std::string filename) visited.\n");
        mFile = fopen(filename.c_str(), "rb");
        jimi_assert(mFile != NULL);
    }

    ~BasicFileOutputStream() {
        printf("01 BasicFileOutputStream<T>::~BasicFileOutputStream() visited.\n");
        close();
    }

    bool valid() {
        return ((mFile != NULL) && (mFile != _INVALID_HANDLE_VALUE));
    }

    void close() {
        printf("10 BasicFileOutputStream<T>::close() visited.\n");
        if (mFile != NULL) {
            fclose(mFile);
            mFile = NULL;
        }
    }

    int available() {
        printf("10 BasicFileOutputStream<T>::available() visited.\n");
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
    int read() { return 0; }
    int read(void * buffer, int size) { return 0; }
    int read(void * buffer, int size, int offset, int len) { return 0; }
};

}  // namespace JsonFx

#endif  /* _JSONFX_STREAM_FILE_OUTPUTSTREAM_H_ */
