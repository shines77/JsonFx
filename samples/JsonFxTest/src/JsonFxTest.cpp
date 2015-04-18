
#include "JsonFxTest.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <intrin.h>

#include "JsonFx/Config.h"
#include "JsonFx/JsonFx.h"

//#include "JsonFx/Stream/InputStream.h"
#include "JsonFx/Stream/FileInputStream.h"

#include "jimic/system/time.h"

// Visual Leak Detector(vld) for Visual C++
//#include "jimi/basic/vld.h"

void JsonFx_Test()
{
#if defined(NDEBUG)
    static const size_t kLoopCount = 200000;
#else
    static const size_t kLoopCount = 1000;
#endif

    JsonFx::FileInputStream fileInputStream;
    fileInputStream.close();
    printf("\n");

    JsonFx::InputStream *inputStream = new JsonFx::FileInputStream();
    inputStream->close();
    printf("\n");

    if (inputStream)
        delete inputStream;
    printf("\n");

    JsonFx::FileInputStreamImpl fileInputStreamImpl;
    fileInputStreamImpl.close();
    printf("\n");

    JsonFx::InputStreamImpl<JsonFx::FileInputStreamImpl> *inputStreamImpl = new JsonFx::FileInputStreamImpl();
    inputStreamImpl->close();
    printf("\n");

    if (inputStreamImpl)
        delete inputStreamImpl;
    printf("\n");

    jmc_timestamp_t starttime;
    jmc_timefloat_t elapsedtime;
    char buffer[4096] = { 0 };
#if 1
    JsonFx::DefaultPoolAllocator poolAllocator(buffer, sizeof(buffer));
#else
    JsonFx::DefaultPoolAllocator poolAllocator;
#endif

    static const char json[] = "{ \"name\": \"wang\", \"sex\": \"male\", \"age\": \"18\" }";
    static const char json_test[] = "[0, 1, 2, 3]";
    JsonFx::Document document(&poolAllocator);

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

    JsonFx::Value value;
    JsonFx::Value val1;
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
    JsonFx::Document document;

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
    JsonFx_Test2();

    JsonFx_BasicDocumentTest1();
    JsonFx_BasicDocumentTest2();

    printf("kUTF8 = %d\n\n", JsonFx::kUTF8);

    system("pause");
    return 0;
}
