
#ifndef _JSONFX_STREAM_IOSTREAMROOT_H_
#define _JSONFX_STREAM_IOSTREAMROOT_H_

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
class BasicIOStreamRoot;

// Define default BasicIOStreamRoot<T>.
typedef BasicIOStreamRoot<_Char>  IOStreamRoot;

template <typename T>
class BasicIOStreamRoot : public internal::Closeable<T>
{
public:
    typedef T       CharType;
    typedef size_t  SizeType;

public:
    // Whether support mark() method?
    static const bool kSupportMarked = false;

public:
    BasicIOStreamRoot() {
        printf("00 BasicIOStreamRoot<T>::BasicIOStreamRoot() visited.\n");
    }

    ~BasicIOStreamRoot() {
        printf("00 BasicIOStreamRoot<T>::~BasicIOStreamRoot() visited.\n");
        close();
    }

    void close() {
        printf("10 BasicIOStreamRoot<T>::close() visited.\n");
    };

    int available() {
        printf("10 BasicIOStreamRoot<T>::available() visited.\n");
        return 0;
    }
    
    bool markSupported() { return kSupportMarked; }
    void mark(int readlimit) {}
    void reset() {}

    size_t skip(size_t n) { return 0; }
};

}  // namespace JsonFx

#endif  /* _JSONFX_STREAM_IOSTREAMROOT_H_ */
