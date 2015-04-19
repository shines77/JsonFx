
#ifndef _JSONFX_INTERNAL_CLOSEABLE_H_
#define _JSONFX_INTERNAL_CLOSEABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Whether use template polymorphism ? */
#define _USE_TEMPLATE_POLYMORPHISM      0

namespace JsonFx {

namespace internal {

class ICloseable {
public:
    virtual void close() = 0;
};

class Closeable {
public:
    Closeable()  {
        printf("00 internal::Closeable::Closeable() visited.\n");
    }

    ~Closeable() {
        printf("01 internal::Closeable::~Closeable() visited.\n");
        close();
    }

    void close() {
        printf("10 internal::Closeable::close() visited.\n");
    }
};

}  // namespace internal

}  // namespace JsonFx

#endif  /* _JSONFX_INTERNAL_CLOSEABLE_H_ */
