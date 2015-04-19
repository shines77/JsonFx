
#ifndef _JSONFX_STREAM_FILE_INPUTSTREAM_H_
#define _JSONFX_STREAM_FILE_INPUTSTREAM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>
#include <string>

#include "jimi/basic/stdsize.h"
#include "jimi/basic/assert.h"

#include "JsonFx/Stream/Detail/FileDef.h"
#include "JsonFx/Stream/InputStream.h"

namespace JsonFx {

// Forward declaration.
template <typename T>
class FileInputStreamBase;

// Define default FileInputStreamBase<T>.
typedef FileInputStreamBase<_Char>  FileInputStream;

template <typename T>
class FileInputStreamBase : public InputStreamBase<T>
{
public:
    typedef typename InputStreamBase<T>::CharType    CharType;
    typedef typename InputStreamBase<T>::SizeType    SizeType;

public:
    static const bool kSupportMarked = true;

private:
    FILE *  mFile;

public:
    FileInputStreamBase() : mFile(NULL) {
        printf("00 FileInputStreamBase<T>::FileInputStreamBase() visited.\n");
    }

    FileInputStreamBase(FILE * hFile) : mFile(hFile) {
        printf("00 FileInputStreamBase<T>::FileInputStreamBase(FILE * hFile) visited.\n");
    }

    FileInputStreamBase(char * filename) : mFile(NULL) {
        printf("00 FileInputStreamBase<T>::FileInputStreamBase(char * filename) visited.\n");
        mFile = fopen(filename, "rb");
    }

    FileInputStreamBase(std::string filename) : mFile(NULL) {
        printf("00 FileInputStreamBase<T>::FileInputStreamBase(std::string filename) visited.\n");
        mFile = fopen(filename.c_str(), "rb");
        jimi_assert(mFile != NULL);
    }

    ~FileInputStreamBase() {
        printf("01 FileInputStreamBase<T>::~FileInputStreamBase() visited.\n");
        close();
    }

    bool valid() {
        return ((mFile != NULL) && (mFile != _INVALID_HANDLE_VALUE));
    }

    void close() {
        printf("10 FileInputStreamBase<T>::close() visited.\n");
        if (mFile != NULL) {
            fclose(mFile);
            mFile = NULL;
        }
    }

    int available() {
        printf("10 FileInputStreamBase<T>::available() visited.\n");
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

#endif  /* _JSONFX_STREAM_FILE_INPUTSTREAM_H_ */
