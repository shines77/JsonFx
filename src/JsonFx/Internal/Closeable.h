
#ifndef _JSONFX_INTERNAL_CLOSEABLE_H_
#define _JSONFX_INTERNAL_CLOSEABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

#include "jimi/basic/stdsize.h"
#include "JsonFx/Internal/AutoCloseable.h"

/* Whether use template polymorphism ? */
#define _USE_TEMPLATE_POLYMORPHISM      0

namespace JsonFx {

namespace internal {

template <typename T>
class ICloseable : public IAutoCloseable<T> {
public:
    typedef T       CharType;
    typedef size_t  SizeType;

public:
    virtual void close() = 0;
};

template <typename T>
class Closeable : public AutoCloseable<T>
{
public:
    typedef T       CharType;
    typedef size_t  SizeType;

public:
    Closeable()  {
        jfx_iostream_trace("00 internal::Closeable<T>::Closeable();\n");
    }

    ~Closeable() {
        jfx_iostream_trace("01 internal::Closeable<T>::~Closeable();\n");
        close();
    }

    void close() {
        jfx_iostream_trace("10 internal::Closeable<T>::close();\n");
    }
};

}  // namespace internal

}  // namespace JsonFx

#endif  /* _JSONFX_INTERNAL_CLOSEABLE_H_ */
