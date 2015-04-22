
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
        printf("00 internal::AutoCloseable<T>::AutoCloseable() visited.\n");
    }

    ~AutoCloseable() {
        printf("01 internal::AutoCloseable<T>::~AutoCloseable() visited.\n");
        close();
    }

    void close() {
        printf("10 internal::AutoCloseable<T>::close() visited.\n");
    }
};

}  // namespace internal

}  // namespace JsonFx

#endif  /* _JSONFX_INTERNAL_AUTOCLOSEABLE_H_ */
