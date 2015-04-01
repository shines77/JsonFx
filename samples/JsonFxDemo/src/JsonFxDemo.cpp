
#include "JsonFxDemo.h"

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include "JsonFx/JsonFx.h"

void JsonFx_Test()
{
    JsonFx::Document document;
    document.parse("[0, 1, 2, 3]");

    if (document.hasMember("test"))
        printf("document.hasMember(\"test\") = true.\n");
    else
        printf("document.hasMember(\"test\") = false.\n");

    JsonFx::Value value;
    value.visit();
}

void JsonFx_BasicDocumentTest()
{
    JsonFx::BasicDocument<JsonFx::CharSet_UTF16> document;
    document.parse(L"[0, 1, 2, 3]");

    JsonFx::BasicValue<JsonFx::CharSet_UTF16> value;
    value.visit();
}

int main(int argn, char * argv[])
{
    JsonFx_Test();

    JsonFx_BasicDocumentTest();

    printf("Hello World!\n\n");
    system("pause");
    return 0;
}
