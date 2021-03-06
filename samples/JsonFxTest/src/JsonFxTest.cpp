
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

#include "jimic/system/time.h"
#include "jimic/system/console.h"

#include "JsonFx/JsonFx.h"

#include "JsonFx/Config.h"
#include "JsonFx/IOStream/InputIOStream.h"
#include "JsonFx/IOStream/FileInputStream.h"
#include "JsonFx/IOStream/FileStream.h"

#include "JsonFx/Stream/StringStream.h"
#include "JsonFx/Stream/StringInputStream.h"

#include "JsonFx/Stream/SizableStringStream.h"
#include "JsonFx/Stream/SizableStringInputStream.h"

// Visual Leak Detector(vld) for Visual C++
//#include "jimi/basic/vld.h"

#if 0
#if defined(_DEBUG) || !defined(NDEBUG)
#pragma comment(lib, "libJsonFx-x86-Debug.lib")
#else
#pragma comment(lib, "libJsonFx-x86-Release.lib")
#endif
#endif

#if 0
#define TEST_JSON_STRING    "{ \"name\": \"wang\", \"sex\": \"male\", \"age\": \"18\" }"
#elif 1
#define TEST_JSON_STRING    "{ \"name\": \"wang\", \"sex\": \"male\\ufa78\", \"age\": \"18\" }"
#else
#define TEST_JSON_STRING    "{\n\"name\": \"wang\",\n\"sex\": \"male\\ufa78\",\n\"age\": \"18\"\n}"
#endif

using namespace JsonFx;

void JsonFx_FastTest()
{
#if defined(_DEBUG) || !defined(NDEBUG)
    static const size_t kLoopCount = 1000;
#else
    static const size_t kLoopCount = 200000;
#endif

    //static char static_buffer[4096] = { 0 };
    //static const DefaultPoolAllocator poolAllocatorConst(buffer, sizeof(buffer));

    jmc_timestamp_t starttime;
    jmc_timefloat_t elapsedtime;
    char buffer[4096] = { 0 };
#if 1
    DefaultPoolAllocator poolAllocator(buffer, sizeof(buffer));
#else
    DefaultPoolAllocator poolAllocator;
#endif

    char json[]   = TEST_JSON_STRING;
    char json_test[] = "[0, 1, 2, 3]";
    Document document(&poolAllocator);
    StringInputStream stringStream(json);

    printf("\n");
    starttime = jmc_get_timestamp();
    for (size_t i = 0; i < kLoopCount; ++i) {
        document.parse(json);
        poolAllocator.reset();
    }
    elapsedtime = jmc_get_elapsedtime_msf(starttime);
    printf("elapsed time: %0.3f ms.\n\n", elapsedtime);
    printf("poolAllocator.getUsed():     %u\n"
           "poolAllocator.getCapacity(): %u\n",
            poolAllocator.getUsed(), poolAllocator.getCapacity());

    printf("\n");
}

void JsonFx_FastTest2()
{
#if defined(_DEBUG) || !defined(NDEBUG)
    static const size_t kLoopCount = 1000;
#else
    static const size_t kLoopCount = 200000;
#endif

    jmc_timestamp_t starttime;
    jmc_timefloat_t elapsedtime;

    char json[] = TEST_JSON_STRING;
    const char json_test[] = "[0, 1, 2, 3]";
    Document document;

    starttime = jmc_get_timestamp();
    for (size_t i = 0; i < kLoopCount; ++i) {
        document.parse(json);
    }
    elapsedtime = jmc_get_elapsedtime_msf(starttime);
    printf("elapsed time: %0.3f ms.\n\n", elapsedtime);
    printf("poolAllocator.getUsed():     %u\n"
           "poolAllocator.getCapacity(): %u\n",
            document.getAllocator()->getUsed(),
            document.getAllocator()->getCapacity());

    printf("\n");
}

void JsonFx_StreamTest()
{
#if defined(_DEBUG) || !defined(NDEBUG)
    static const size_t kLoopCount = 1000;
#else
    static const size_t kLoopCount = 200000;
#endif

    //static char static_buffer[4096] = { 0 };
    //static const DefaultPoolAllocator poolAllocatorConst(buffer, sizeof(buffer));

    jmc_timestamp_t starttime;
    jmc_timefloat_t elapsedtime;
    char buffer[4096] = { 0 };
#if 1
    DefaultPoolAllocator poolAllocator(buffer, sizeof(buffer));
#else
    DefaultPoolAllocator poolAllocator;
#endif

    char json[] = TEST_JSON_STRING;
    char json_test[] = "[0, 1, 2, 3]";
    Document document(&poolAllocator);
    StringInputStream stringStream(json);

    starttime = jmc_get_timestamp();
    for (size_t i = 0; i < kLoopCount; ++i) {
        document.parseStream(json);
        /*
        document.parse<>(json);
        document.parse<StringInputStream>(json);
        document.parse<SizableStringInputStream>(json);

        document.parse(stringStream);
        document.parse<>(stringStream);
        document.parse<StringInputStream>(stringStream);
        document.parse< BasicStringInputStream<char> >(stringStream);
        document.parse<0>(stringStream);
        document.parse<0, CharSet::UTF8>(stringStream);
        document.parse<0, StringInputStream>(stringStream);
        document.parse<0, CharSet::UTF8, StringInputStream>(stringStream);
        //*/
        poolAllocator.reset();
    }
    elapsedtime = jmc_get_elapsedtime_msf(starttime);
    printf("elapsed time: %0.3f ms.\n\n", elapsedtime);
    printf("poolAllocator.getUsed():     %u\n"
           "poolAllocator.getCapacity(): %u\n\n",
            poolAllocator.getUsed(), poolAllocator.getCapacity());

    document.visit();

#if 0
    if (document.hasMember("test"))
        printf("document.hasMember(\"test\") = true.\n");
    else
        printf("document.hasMember(\"test\") = false.\n");

    document.test();

    Value value;
    value.visit();
#endif
}

void JsonFx_StreamTest2()
{
#if defined(_DEBUG) || !defined(NDEBUG)
    static const size_t kLoopCount = 1000;
#else
    static const size_t kLoopCount = 200000;
#endif

    jmc_timestamp_t starttime;
    jmc_timefloat_t elapsedtime;

    char json[] = TEST_JSON_STRING;
    const char json_test[] = "[0, 1, 2, 3]";
    Document document;

    starttime = jmc_get_timestamp();
    for (size_t i = 0; i < kLoopCount; ++i) {
        document.parseStream(json);
    }
    elapsedtime = jmc_get_elapsedtime_msf(starttime);
    printf("elapsed time: %0.3f ms.\n\n", elapsedtime);
    printf("poolAllocator.getUsed():     %u\n"
           "poolAllocator.getCapacity(): %u\n",
            document.getAllocator()->getUsed(),
            document.getAllocator()->getCapacity());

    printf("\n");
}

void JsonFx_BasicDocumentTest1()
{
    BasicDocument<CharSet::UTF16> document;
    document.parse(L"[0, 1, 2, 3]");
    document.visit();

    BasicValue<CharSet::UTF16> value;
    //value.visit();
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
    char json[] = TEST_JSON_STRING;

    jfx_iostream_trace("%d\n", 1);

    printf("=====================================================\n");
#if 0
    StringStream strStream(json);
    printf("The StringStream buffer is:\n");
    while (!strStream.isEof()) {
        printf("%c", strStream.get());
        strStream.next();
        strStream.nextw();
        strStream.tell();
        strStream.tellw();
        strStream.isReadOverflow();
        strStream.isWriteOverflow();
    }
    printf("\n");
    printf("The buffer length is: %d", strStream.tell());
    printf("\n");
#endif

    StringInputStream strInputStream(json);
    printf("The StringInputStream buffer is:\n");
    while (!strInputStream.isEof()) {
        printf("%c", strInputStream.get());
        strInputStream.next();
        strInputStream.tell();
        strInputStream.isReadOverflow();
    }
    printf("\n");
    printf("The buffer length is: %d", strInputStream.tell());
    printf("\n");

    printf("=====================================================\n");
}

void JsonFx_SizableStringStream_Test()
{
    static const char json[] = TEST_JSON_STRING;

    jfx_iostream_trace("%d\n", 1);

    printf("=====================================================\n\n");

    SizableStringStream strStream(json, _countof(json));
    printf("The SizableStringStream buffer is:\n\n");
    while (!strStream.isEof()) {
        printf("%c", strStream.get());
        strStream.next();
    }
    printf("\n\n");
    printf("The buffer length is: %d\n", strStream.tell());
    printf("\n");

    SizableStringInputStream strInputStream(json, _countof(json));
    printf("The SizableStringInputStream buffer is:\n\n");
    while (!strInputStream.isEof()) {
        printf("%c", strInputStream.get());
        strInputStream.next();
    }
    printf("\n\n");
    printf("The buffer length is: %d\n", strInputStream.tell());
    printf("\n");

    printf("=====================================================\n");
}

int main(int argn, char * argv[])
{
    //Json json;
    //json.visit();

    JsonFx_FastTest();
    JsonFx_FastTest2();
    JsonFx_StreamTest();
    JsonFx_StreamTest2();

    JsonFx_BasicDocumentTest1();
    //JsonFx_BasicDocumentTest2();

    //JsonFx_IOStream_Test();
    JsonFx_StringStream_Test();
    //JsonFx_SizableStringStream_Test();

    //printf("\n");
    //printf("kUTF8 = %d\n\n", kUTF8);

    jimi_console_readkeyln(false, true, false);
    return 0;
}
