
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
#include "jimi/basic/vld.h"

#if 0
#if defined(_DEBUG) || !defined(NDEBUG)
#pragma comment(lib, "libJsonFx-x86-Debug.lib")
#else
#pragma comment(lib, "libJsonFx-x86-Release.lib")
#endif
#endif

using namespace JsonFx;

void JsonFx_Test()
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

    static const char json[] = "{ \"name\": \"wang\", \"sex\": \"male\", \"age\": \"18\" }";
    static const char json_test[] = "[0, 1, 2, 3]";
    Document document(&poolAllocator);
    StringInputStream stringStream(json);

    printf("\n");
    starttime = jmc_get_timestamp();
    for (size_t i = 0; i < kLoopCount; ++i) {
        document.parse(json);
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
#if defined(_DEBUG) || !defined(NDEBUG)
    static const size_t kLoopCount = 1000;
#else
    static const size_t kLoopCount = 200000;
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

    printf("=====================================================\n\n");

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

    printf("=====================================================\n");
}

void JsonFx_SizableStringStream_Test()
{
    static const char json[] = "{ \"name\": \"wang\", \"sex\": \"male\", \"age\": \"18\" }";

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

struct foo_t
{
    foo_t(int i) : v(i) {}
    ~foo_t() {}

    void foo() {}

    int v;
};

int main(int argn, char * argv[])
{
    Json json;
    //json.visit();

    std::shared_ptr<foo_t *> p = std::make_shared<foo_t *>(new foo_t(5));
    foo_t ** i = p.get();
    (*i)->foo();

    std::shared_ptr<foo_t> p2 = std::make_shared<foo_t>(5);
    foo_t * i2 = p2.get();
    i2->foo();

    JsonFx_Test();
    JsonFx_Test2();

    JsonFx_BasicDocumentTest1();
    JsonFx_BasicDocumentTest2();

    JsonFx_IOStream_Test();
    //JsonFx_StringStream_Test();
    //JsonFx_SizableStringStream_Test();

    //printf("\n");
    //printf("kUTF8 = %d\n\n", kUTF8);

    jimi_console_readkeyln(false, true, false);
    return 0;
}
