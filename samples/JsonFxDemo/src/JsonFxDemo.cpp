
#include "JsonFxDemo.h"

#include <stdlib.h>
#include <stdio.h>

#include "JsonFx/JsonFx.h"

void JsonFx_Test()
{
    JsonFx::Value value;
    value.get();
}

int main(int argn, char * argv[])
{
    JsonFx_Test();

    printf("Hello World!\n\n");
    system("pause");
    return 0;
}
