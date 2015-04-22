
#ifndef _JSONFX_STREAM_FILESTREAM_H_
#define _JSONFX_STREAM_FILESTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>
#include <string>

#include "jimi/basic/stdsize.h"
#include "jimi/basic/assert.h"

#include "JsonFx/Stream/Detail/FileDef.h"
#include "JsonFx/Stream/FileInputStream.h"
#include "JsonFx/Stream/FileOutputStream.h"

namespace JsonFx {

// Forward declaration.
template <typename T = JSONFX_DEFAULT_CHARTYPE>
class BasicFileStream;

// Define default BasicFileStream<T>.
typedef BasicFileStream<>  FileStream;

template <typename T>
class BasicFileStream : public BasicFileInputStream<T>,
                        public BasicFileOutputStream<T>
{
public:
    typedef typename BasicFileInputStream<T>::CharType    CharType;
    typedef typename BasicFileInputStream<T>::SizeType    SizeType;

public:
    static const bool kSupportMarked = true;

private:
    FILE *  mFile;

public:
    BasicFileStream() : mFile(NULL) {
        printf("00 BasicFileStream<T>::BasicFileStream() visited.\n");
    }

    BasicFileStream(FILE * hFile) : mFile(hFile) {
        printf("00 BasicFileStream<T>::BasicFileStream(FILE * hFile) visited.\n");
    }

    BasicFileStream(char * filename) : mFile(NULL) {
        printf("00 BasicFileStream<T>::BasicFileStream(char * filename) visited.\n");
        mFile = fopen(filename, "rb");
    }

    BasicFileStream(std::string filename) : mFile(NULL) {
        printf("00 BasicFileStream<T>::BasicFileStream(std::string filename) visited.\n");
        mFile = fopen(filename.c_str(), "rb");
        jimi_assert(mFile != NULL);
    }

    ~BasicFileStream() {
        printf("01 BasicFileStream<T>::~BasicFileStream() visited.\n");
        close();
    }

    bool valid() {
        return ((mFile != NULL) && (mFile != _INVALID_HANDLE_VALUE));
    }

    void close() {
        printf("10 BasicFileStream<T>::close() visited.\n");
        if (mFile != NULL) {
            fclose(mFile);
            mFile = NULL;
        }
    }

    int available() {
        printf("10 BasicFileStream<T>::available() visited.\n");
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

#endif  /* _JSONFX_STREAM_FILESTREAM_H_ */
