#ifndef HYPERTEST_TEST_H
#define HYPERTEST_TEST_H
#pragma once

// TODO: Clean this mess up.
#define TEST(SET, NAME) \
    extern "C" struct test_state TestState; \
    OM_INTERNAL void Test_Run_##SET##_##NAME(int *Result);\
    OM_INTERNAL void Test_##SET##_##NAME(int *Result, size_t TestIndex) \
    { \
        (void)TestIndex;\
        Test_Run_##SET##_##NAME(Result);\
    } \
    UNITTEST_INITIALIZER(Test_Register_##SET##_##NAME) \
    {\
        size_t Index = TestState.NumberOfTests++;\
        const char *NamePart = #SET "." #NAME; \
        u32 NameSize = strlen(NamePart) + 1; \
        char *Name = (char *) malloc(NameSize); \
        TestState.Tests =                                                        \
        (struct test *) realloc((void *) TestState.Tests,      \
                               sizeof(struct test) * TestState.NumberOfTests); \
        TestState.Tests[Index].Function = &Test_##SET##_##NAME; \
        TestState.Tests[Index].Name = Name; \
        sprintf(Name, "%s", NamePart); \
    }\
    void Test_Run_##SET##_##NAME(int *Result)

#endif //HYPERTEST_TEST_H