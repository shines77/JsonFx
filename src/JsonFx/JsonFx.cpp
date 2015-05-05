
#include "JsonFx.h"

#include <stdio.h>

namespace JsonFx {

#if defined(JSONFX_STATIC_LIB)

void Json::visit()
{
    printf("JsonFx::Json::visit() enter.\n");
    printf("JsonFx::Json::visit() over.\n\n");
}

#endif

}  // namespace JsonFx
