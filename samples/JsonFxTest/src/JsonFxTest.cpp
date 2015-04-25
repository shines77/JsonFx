
#include "JsonFxTest.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <intrin.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>
#include <strstream>
#include <streambuf>

#include "jimic/system/time.h"
#include "jimic/system/console.h"

#include "JsonFx/JsonFx.h"

#include "JsonFx/Config.h"
#include "JsonFx/IOStream/InputIOStream.h"
#include "JsonFx/IOStream/FileInputStream.h"
#include "JsonFx/IOStream/FileStream.h"

#include "JsonFx/Stream/StringStream.h"
#include "JsonFx/Stream/StringInputStream.h"

// Visual Leak Detector(vld) for Visual C++
//#include "jimi/basic/vld.h"

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

void JsonFx_IOStream_Test()
{
    FileStream fileStream;
    fileStream.close();
    jfx_iostream_trace("\n");

    FileInputStream fileInputStream;
    fileInputStream.close();
    jfx_iostream_trace("\n");

    InputIOStream *inputIOStream = new FileInputStream();
    inputIOStream->close();
    jfx_iostream_trace("\n");

    if (inputIOStream)
        delete inputIOStream;
    jfx_iostream_trace("\n");

#if 0
    const char test_str[] = "This is a test string.";
    char buf[64];

    std::ios ios1;
    std::ios_base ios_base1;
    std::iostream iostream1;
    std::istream is;
#endif
}

void JsonFx_StringStream_Test()
{
    static const char json[] = "{ \"name\": \"wang\", \"sex\": \"male\", \"age\": \"18\" }";

    jfx_iostream_trace("%d\n", 1);

    StringStream strStream(json);
    printf("The StringStream buffer is:\n\n");
    while (!strStream.isEof()) {
        printf("%c", strStream.get());
        strStream.next();
    }
    printf("\n\n");
    printf("The buffer length is: %d\n", strStream.tell());
    printf("\n");

    StringInputStream strInputStream(json);
    printf("The StringInputStream buffer is:\n\n");
    while (!strInputStream.isEof()) {
        printf("%c", strInputStream.get());
        strInputStream.next();
    }
    printf("\n\n");
    printf("The buffer length is: %d\n", strInputStream.tell());
    printf("\n");
}

int main(int argn, char * argv[])
{
    JsonFx_Test();
    JsonFx_Test2();

    JsonFx_BasicDocumentTest1();
    JsonFx_BasicDocumentTest2();

    JsonFx_IOStream_Test();
    JsonFx_StringStream_Test();

    printf("kUTF8 = %d\n\n", kUTF8);

    jimi_console_readkeyln(false, true, false);
    return 0;
}
