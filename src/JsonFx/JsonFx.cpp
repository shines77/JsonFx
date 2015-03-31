
#include "JsonFx.h"

#include <stdio.h>

namespace JsonFx {

void Json::visit()
{
    printf("JsonFx::Json::visit() enter.\n");
    printf("JsonFx::Json::visit() over.\n\n");
}

}  // namespace JsonFx
