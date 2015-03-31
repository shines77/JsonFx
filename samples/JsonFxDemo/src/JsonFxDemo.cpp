
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

    JsonFx::Value value;;
    value.visit();
}

void JsonFx_BaseDocumentTest()
{
    JsonFx::BaseDocument<JsonFx::CharSet::UTF16> document;
    document.parse(L"[0, 1, 2, 3]");

    JsonFx::BaseValue<JsonFx::CharSet::UTF16> value;
    value.visit();
}

int main(int argn, char * argv[])
{
    JsonFx_Test();

    JsonFx_BaseDocumentTest();

    printf("Hello World!\n\n");
    system("pause");
    return 0;
}
