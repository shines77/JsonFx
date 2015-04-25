
#ifndef _JSONFX_INTERNAL_AUTOCLOSEABLE_H_
#define _JSONFX_INTERNAL_AUTOCLOSEABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>
#include "jimi/basic/stdsize.h"

namespace JsonFx {

namespace internal {

template <typename T>
class IAutoCloseable {
public:
    typedef T       CharType;
    typedef size_t  SizeType;

public:
    virtual void close() = 0;
};

template <typename T>
class AutoCloseable {
public:
    typedef T       CharType;
    typedef size_t  SizeType;

public:
    AutoCloseable()  {
        jfx_iostream_trace("00 internal::AutoCloseable<T>::AutoCloseable();\n");
    }

    ~AutoCloseable() {
        jfx_iostream_trace("01 internal::AutoCloseable<T>::~AutoCloseable();\n");
        close();
    }

    void close() {
        jfx_iostream_trace("10 internal::AutoCloseable<T>::close();\n");
    }
};

}  // namespace internal

}  // namespace JsonFx

#endif  /* _JSONFX_INTERNAL_AUTOCLOSEABLE_H_ */
