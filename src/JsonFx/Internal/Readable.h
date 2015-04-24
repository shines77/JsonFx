
#ifndef _JSONFX_INTERNAL_READABLE_H_
#define _JSONFX_INTERNAL_READABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>
#include "jimi/basic/stdsize.h"

namespace JsonFx {

namespace internal {

template <typename T>
class IReadable {
public:
    typedef T       CharType;
    typedef size_t  SizeType;

public:
    virtual int read() = 0;
    virtual int read(CharType & c) = 0;
    virtual int read(void * buffer, int size) = 0;
    virtual int read(void * buffer, int size, int offset, int len) = 0;
};

template <typename T>
class Readable {
public:
    typedef T       CharType;
    typedef size_t  SizeType;

public:
    Readable()  {
        jfx_iostream_trace("00 internal::Readable::Readable() visited.\n");
    }

    ~Readable() {
        jfx_iostream_trace("01 internal::Readable::~Readable() visited.\n");
    }

    int read() {
        jfx_iostream_trace("10 internal::Readable::read() visited.\n");
    }

    int read(CharType & c) {
        jfx_iostream_trace("10 internal::Readable::read(char &c); visited.\n");
    }

    int read(void * buffer, int size) {
        jfx_iostream_trace("10 internal::Readable::read(void *buffer, int size); visited.\n");
    }

    int read(void * buffer, int size, int offset, int len) {
        jfx_iostream_trace("10 internal::Readable::read(void *buffer, int size, int offset, int len); visited.\n");
    }
};

}  // namespace internal

}  // namespace JsonFx

#endif  /* _JSONFX_INTERNAL_READABLE_H_ */
