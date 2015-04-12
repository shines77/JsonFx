
#include "JsonFxTest.h"

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <memory.h>
#include <intrin.h>

#include "JsonFx/JsonFx.h"

#include "jimic/system/time.h"

void JsonFx_Test()
{
    JsonFx::Document document;
    document.parse("[0, 1, 2, 3]");
    document.visit();

    if (document.hasMember("test"))
        printf("document.hasMember(\"test\") = true.\n");
    else
        printf("document.hasMember(\"test\") = false.\n");

    document.test();

    JsonFx::Value value;
    JsonFx::Value val1;
    value.visit();
}

void JsonFx_BasicDocumentTest1()
{
    JsonFx::BasicDocument<JsonFx::CharSet::UTF16> document;
    document.parse(L"[0, 1, 2, 3]");
    document.visit();

    JsonFx::BasicValue<JsonFx::CharSet::UTF16> value;
    value.visit();
}

void JsonFx_BasicDocumentTest2()
{
    JsonFx::BasicDocument<JsonFx::_UTF16> document;
    document.parse(L"[0, 1, 2, 3]");
    document.visit();

    JsonFx::BasicValue<JsonFx::_UTF16> value;
    value.visit();
}

int main(int argn, char * argv[])
{
    JsonFx_Test();

    JsonFx_BasicDocumentTest1();
    JsonFx_BasicDocumentTest2();

    printf("kUTF8 = %d\n\n", JsonFx::kUTF8);

    system("pause");
    return 0;
}
