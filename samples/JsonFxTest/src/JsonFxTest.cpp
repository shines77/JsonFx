
#include "JsonFxTest.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <intrin.h>

#include "jimic/system/time.h"

#include "JsonFx/JsonFx.h"

#include "JsonFx/Config.h"
#include "JsonFx/Stream/InputStream.h"
#include "JsonFx/Stream/FileInputStream.h"
#include "JsonFx/Stream/FileStream.h"

// Visual Leak Detector(vld) for Visual C++
//#include "jimi/basic/vld.h"\

using namespace JsonFx;

void JsonFx_Test()
{
#if defined(NDEBUG)
    static const size_t kLoopCount = 200000;
#else
    static const size_t kLoopCount = 1000;
#endif

    jmc_timestamp_t starttime;
    jmc_timefloat_t elapsedtime;
    char buffer[4096] = { 0 };
#if 1
    DefaultPoolAllocator poolAllocator(buffer, sizeof(buffer));
#else
    DefaultPoolAllocator poolAllocator;
#endif

    static const char json[] = "{ \"name\": \"wang\", \"sex\": \"male\", \"age\": \"18\" }";
    static const char json_test[] = "[0, 1, 2, 3]";
    Document document(&poolAllocator);

    printf("\n");
    starttime = jmc_get_timestamp();
    for (size_t i = 0; i < kLoopCount; ++i) {
        document.parse(json);
        poolAllocator.reset();
    }
    elapsedtime = jmc_get_elapsedtime_msf(starttime);
    printf("elapsed time: %0.3f ms.\n\n", elapsedtime);
    printf("poolAllocator.getUsed():     %u\n"
           "poolAllocator.getCapacity(): %u\n\n",
            poolAllocator.getUsed(), poolAllocator.getCapacity());

    document.visit();

    if (document.hasMember("test"))
        printf("document.hasMember(\"test\") = true.\n");
    else
        printf("document.hasMember(\"test\") = false.\n");

    document.test();

    Value value;
    Value val1;
    value.visit();
}

void JsonFx_Test2()
{
#if defined(NDEBUG)
    static const size_t kLoopCount = 200000;
#else
    static const size_t kLoopCount = 1000;
#endif

    jmc_timestamp_t starttime;
    jmc_timefloat_t elapsedtime;

    static const char json[] = "{ \"name\": \"wang\", \"sex\": \"male\", \"age\": \"18\" }";
    static const char json_test[] = "[0, 1, 2, 3]";
    Document document;

    starttime = jmc_get_timestamp();
    for (size_t i = 0; i < kLoopCount; ++i) {
        document.parse(json);
    }
    elapsedtime = jmc_get_elapsedtime_msf(starttime);
    printf("elapsed time: %0.3f ms.\n\n", elapsedtime);
    printf("poolAllocator.getUsed():     %u\n"
           "poolAllocator.getCapacity(): %u\n\n",
            document.getAllocator()->getUsed(),
            document.getAllocator()->getCapacity());

    document.visit();

    if (document.hasMember("test"))
        printf("document.hasMember(\"test\") = true.\n");
    else
        printf("document.hasMember(\"test\") = false.\n");

    document.test();

    Value value;
    Value val1;
    value.visit();
}

void JsonFx_BasicDocumentTest1()
{
    BasicDocument<CharSet::UTF16> document;
    document.parse(L"[0, 1, 2, 3]");
    document.visit();

    BasicValue<CharSet::UTF16> value;
    value.visit();
}

void JsonFx_BasicDocumentTest2()
{
    BasicDocument<_UTF16> document;
    document.parse(L"[0, 1, 2, 3]");
    document.visit();

    BasicValue<_UTF16> value;
    value.visit();
}

void JsonFx_Stream_Test()
{
    FileStream fileStream;
    fileStream.close();
    printf("\n");

    FileInputStream fileInputStream;
    fileInputStream.close();
    printf("\n");

    InputStream *inputStream = new FileInputStream();
    inputStream->close();
    printf("\n");

    if (inputStream)
        delete inputStream;
    printf("\n");
}

int main(int argn, char * argv[])
{
    JsonFx_Test();
    JsonFx_Test2();

    JsonFx_BasicDocumentTest1();
    JsonFx_BasicDocumentTest2();

    //JsonFx_Stream_Test();

    printf("kUTF8 = %d\n\n", kUTF8);

    system("pause");
    return 0;
}
