
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
class FileOutputStreamBase;

// Define default FileOutputStreamBase<T>.
typedef FileOutputStreamBase<_Char>  FileOutputStream;

template <typename T>
class FileOutputStreamBase : public InputStreamBase<T>
{
public:
    typedef typename InputStreamBase<T>::CharType    CharType;
    typedef typename InputStreamBase<T>::SizeType    SizeType;

public:
    static const bool kSupportMarked = true;

private:
    FILE *  mFile;

public:
    FileOutputStreamBase() : mFile(NULL) {
        printf("00 FileOutputStreamBase<T>::FileOutputStreamBase() visited.\n");
    }

    FileOutputStreamBase(FILE * hFile) : mFile(hFile) {
        printf("00 FileOutputStreamBase<T>::FileOutputStreamBase(FILE * hFile) visited.\n");
    }

    FileOutputStreamBase(char * filename) : mFile(NULL) {
        printf("00 FileOutputStreamBase<T>::FileOutputStreamBase(char * filename) visited.\n");
        mFile = fopen(filename, "rb");
    }

    FileOutputStreamBase(std::string filename) : mFile(NULL) {
        printf("00 FileOutputStreamBase<T>::FileOutputStreamBase(std::string filename) visited.\n");
        mFile = fopen(filename.c_str(), "rb");
        jimi_assert(mFile != NULL);
    }

    ~FileOutputStreamBase() {
        printf("01 FileOutputStreamBase<T>::~FileOutputStreamBase() visited.\n");
        close();
    }

    bool valid() {
        return ((mFile != NULL) && (mFile != _INVALID_HANDLE_VALUE));
    }

    void close() {
        printf("10 FileOutputStreamBase<T>::close() visited.\n");
        if (mFile != NULL) {
            fclose(mFile);
            mFile = NULL;
        }
    }

    int available() {
        printf("10 FileOutputStreamBase<T>::available() visited.\n");
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

#endif  /* _JSONFX_STREAM_FILE_OUTPUTSTREAM_H_ */
