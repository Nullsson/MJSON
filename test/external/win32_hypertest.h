#ifndef WIN32_HYPERTEST_H
#define WIN32_HYPERTEST_H
#pragma once

// NOTE: CRT hack
#if defined(_MSC_VER)
#pragma section(".CRT$XCU", read)
#define UNITTEST_INITIALIZER(f)                                                \
  static void __cdecl f(void);                                                 \
  __declspec(allocate(".CRT$XCU")) void(__cdecl * f##_)(void) = f;             \
static void __cdecl f(void)
#endif

typedef void (*Unittest_Testcase)(int *, size_t);

struct test
{
    Unittest_Testcase Function;
    size_t Index;
    char *Name;
};

struct test_state
{
    struct test *Tests;
    size_t NumberOfTests;
    FILE *Output;
};

#endif //WIN32_HYPERTEST_H