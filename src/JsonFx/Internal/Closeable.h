
#ifndef _JSONFX_INTERNAL_CLOSEABLE_H_
#define _JSONFX_INTERNAL_CLOSEABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Whether use template polymorphism ? */
#define _USE_TEMPLATE_POLYMORPHISM      0

namespace JsonFx {

namespace internal {

class Closeable {
public:
    virtual void close() = 0;
};

class CloseableImpl {
public:
    CloseableImpl()  {
        printf("00 internal::CloseableImpl::CloseableImpl() visited.\n");
    }

    ~CloseableImpl() {
        printf("01 internal::CloseableImpl::~CloseableImpl() visited.\n");
        close();
    }

    void close() {
        printf("10 internal::CloseableImpl::close() visited.\n");
    }
};

}  // namespace internal

}  // namespace JsonFx

#endif  /* _JSONFX_INTERNAL_CLOSEABLE_H_ */
