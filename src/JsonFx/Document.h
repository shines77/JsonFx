
#ifndef _JSONFX_DOCUMENT_H_
#define _JSONFX_DOCUMENT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>

namespace JsonFx {

class Document {
public:
    Document()  {}
    ~Document() {}

    void get();
};

void Document::get()
{
    printf("JsonFx::Document::get() visited.\n\n");
}

}  // namespace JsonFx

#endif  /* !_JSONFX_DOCUMENT_H_ */
