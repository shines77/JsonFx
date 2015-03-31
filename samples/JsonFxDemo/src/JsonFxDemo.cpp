
#include "JsonFxDemo.h"

#include <stdlib.h>
#include <stdio.h>

#include "JsonFx/JsonFx.h"

void JsonFx_Test()
{
    JsonFx::Document document;
    document.parse("[0, 1, 2, 3]");

    JsonFx::Value value;
    //value.mValueType = 1;
    value.visit();
}

int main(int argn, char * argv[])
{
    JsonFx_Test();

    printf("Hello World!\n\n");
    system("pause");
    return 0;
}
