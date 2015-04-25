
#ifndef _JSONFX_INTERNAL_WRITEABLE_H_
#define _JSONFX_INTERNAL_WRITEABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>
#include "jimi/basic/stdsize.h"

namespace JsonFx {

namespace internal {

template <typename T>
class IWriteable {
public:
    typedef T       CharType;
    typedef size_t  SizeType;

public:
    virtual int write(CharType c) = 0;
    virtual int write(void * buffer, int size) = 0;
    virtual int write(void * buffer, int size, int offset, int len) = 0;
};

template <typename T>
class Writeable {
public:
    typedef T       CharType;
    typedef size_t  SizeType;

public:
    Writeable()  {
        jfx_iostream_trace("00 internal::Writeable::Writeable();\n");
    }

    ~Writeable() {
        jfx_iostream_trace("01 internal::Writeable::~Writeable();\n");
    }

    int write(CharType c) {
        jfx_iostream_trace("10 internal::Writeable::write(char c);\n");
    }

    int write(void * buffer, int size) {
        jfx_iostream_trace("10 internal::Writeable::write(void *buffer, int size);\n");
    }

    int write(void * buffer, int size, int offset, int len) {
        jfx_iostream_trace("10 internal::Writeable::write(void *buffer, int size, int offset, int len);\n");
    }
};

}  // namespace internal

}  // namespace JsonFx

#endif  /* _JSONFX_INTERNAL_WRITEABLE_H_ */
