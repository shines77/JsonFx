
#ifndef _JSONFX_FILE_INPUTSTREAM2_H_
#define _JSONFX_FILE_INPUTSTREAM2_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>
#include <string>

//#include "jimi/basic/stddef.h"
#include "jimi/basic/stdsize.h"
#include "jimi/basic/assert.h"

#ifndef _INVALID_HANDLE_VALUE
#define _INVALID_HANDLE_VALUE    ((void *)(-1))
#define _INVALID_HANDLE_VALUE_   ((HANDLE)(LONG_PTR)-1)
#endif

#include "JsonFx/Stream/InputStream.h"

//using namespace std;

namespace JsonFx {

class FileInputStream : public InputStream
{
public:
    static const bool kSupportMarded = true;

private:
    FILE *  mFile;

public:
    FileInputStream() : mFile(NULL) {
        printf("00 FileInputStream::FileInputStream() visited.\n");
    }

    FileInputStream(FILE * hFile) : mFile(hFile) {
        printf("00 FileInputStream::FileInputStream(FILE * hFile) visited.\n");
    }

    FileInputStream(char * filename) : mFile(NULL) {
        printf("00 FileInputStream::FileInputStream(char * filename) visited.\n");
        mFile = fopen(filename, "rb");
    }

    FileInputStream(std::string filename) : mFile(NULL) {
        printf("00 FileInputStream::FileInputStream(std::string filename) visited.\n");
        mFile = fopen(filename.c_str(), "rb");
        jimi_assert(mFile != NULL);
    }

    virtual ~FileInputStream() {
        printf("01 FileInputStream::~FileInputStream() visited.\n");
        close();
    }

    bool valid() {
        return ((mFile != NULL) && (mFile != _INVALID_HANDLE_VALUE));
    }

    virtual void close() {
        printf("10 FileInputStream::close() visited.\n");
        if (mFile != NULL) {
            fclose(mFile);
            mFile = NULL;
        }
    }

    virtual int available() { return 0; }
    
    virtual void reset() {}
    virtual size_t skip(size_t n) { return 0; }

    virtual void mark(int readlimit) {}
    virtual bool markSupported() { return kSupportMarded; }

    /**
     * Reads the next byte of data from the input stream. The value byte is returned as an int in the range 0 to 255.
     * If no byte is available because the end of the stream has been reached, the value -1 is returned.
     * This method blocks until input data is available, the end of the stream is detected, or an exception is thrown.
     */
    virtual int read() { return 0; }
    virtual int read(void * buffer, int size) { return 0; }
    virtual int read(void * buffer, int size, int offset, int len) { return 0; }
};

class FileInputStreamImpl : public InputStreamImpl<FileInputStreamImpl>
{
public:
    static const bool kSupportMarded = true;

private:
    FILE *  mFile;

public:
    FileInputStreamImpl() : mFile(NULL) {
        printf("00 FileInputStreamImpl::FileInputStreamImpl() visited.\n");
    }

    FileInputStreamImpl(FILE * hFile) : mFile(hFile) {
        printf("00 FileInputStreamImpl::FileInputStreamImpl(FILE * hFile) visited.\n");
    }

    FileInputStreamImpl(char * filename) : mFile(NULL) {
        printf("00 FileInputStreamImpl::FileInputStreamImpl(char * filename) visited.\n");
        mFile = fopen(filename, "rb");
    }

    FileInputStreamImpl(std::string filename) : mFile(NULL) {
        printf("00 FileInputStreamImpl::FileInputStreamImpl(std::string filename) visited.\n");
        mFile = fopen(filename.c_str(), "rb");
        jimi_assert(mFile != NULL);
    }

    ~FileInputStreamImpl() {
        printf("01 FileInputStreamImpl::~FileInputStreamImpl() visited.\n");
        close();
    }

    bool valid() {
        return ((mFile != NULL) && (mFile != _INVALID_HANDLE_VALUE));
    }

    void close_impl() {
        printf("10 FileInputStreamImpl::close_impl() visited.\n");
        if (mFile != NULL) {
            fclose(mFile);
            mFile = NULL;
        }
    }

    int available() {
        printf("10 FileInputStreamImpl::available() visited.\n");
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
};

}  // namespace JsonFx

#endif  /* _JSONFX_FILE_INPUTSTREAM2_H_ */
