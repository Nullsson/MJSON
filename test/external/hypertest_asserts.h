#ifndef HYPERTEST_ASSERTS_H
#define HYPERTEST_ASSERTS_H
#pragma once

// TODO: Extend this macro if we want to include file logging.
#define UNITTEST_PRINTF(...) \
    printf(__VA_ARGS__)

#define UNITTEST_ASSERT(X, Y, Condition) \
    { \
        if (!((X)Condition(Y))) \
        { \
            UNITTEST_PRINTF("%s:%u: Failed\n", __FILE__, __LINE__); \
            *Result = 1; \
            return; \
        } \
    }

#define UNITTEST_ASSERT_TRUE(X) \
    if (!(X)) \
    { \
        UNITTEST_PRINTF("%s:%u: Failed\n", __FILE__, __LINE__); \
        UNITTEST_PRINTF("  Expected : True\n"); \
        UNITTEST_PRINTF("    Actual : %s\n", (X) ? "True" : "False"); \
        *Result = 1;\
        return;\
    }

#define UNITTEST_ASSERT_FALSE(X) \
    if (X) \
    { \
        UNITTEST_PRINTF("%s:%u: Failed\n", __FILE__, __LINE__); \
        UNITTEST_PRINTF("  Expected : False\n"); \
        UNITTEST_PRINTF("    Actual : %s\n", (X) ? "True" : "False"); \
        *Result = 1;\
        return;\
    }

#define UNITTEST_ASSERT_EQUAL(X, Y) UNITTEST_ASSERT(X, Y, ==)
#define UNITTEST_ASSERT_NOTEQUAL(X, Y) UNITTEST_ASSERT(X, Y, !=)
#define UNITTEST_ASSERT_LESSTHAN(X, Y) UNITTEST_ASSERT(X, Y, <)
#define UNITTEST_ASSERT_LESSOREQUAL(X, Y) UNITTEST_ASSERT(X, Y, <=)
#define UNITTEST_ASSERT_GREATERTHAN(X, Y) UNITTEST_ASSERT(X, Y, >)
#define UNITTEST_ASSERT_GREATEROREQUAL(X, Y) UNITTEST_ASSERT(X, Y, =>)

#define UNITTEST_EXPECT(X, Y, Condition) \
    { \
        if (!((X)Condition(Y))) \
        { \
            UNITTEST_PRINTF("%s:%u: Failed\n", __FILE__, __LINE__); \
            *Result = 1; \
        } \
    }

#define UNITTEST_EXPECT_TRUE(X) \
    if (!(X)) \
    { \
        UNITTEST_PRINTF("%s:%u: Failed\n", __FILE__, __LINE__); \
        UNITTEST_PRINTF("  Expected : true\n"); \
        UNITTEST_PRINTF("    Actual : %s\n", (X) ? "True" : "False"); \
        *Result = 1; \
    }

#define UNITTEST_EXPECT_FALSE(X) \
    if (X) \
    { \
        UNITTEST_PRINTF("%s:%u: Failed\n", __FILE__, __LINE__); \
        UNITTEST_PRINTF("  Expected : true\n"); \
        UNITTEST_PRINTF("    Actual : %s\n", (X) ? "True" : "False"); \
        *Result = 1; \
    }

#define UNITTEST_EXPECT_EQUAL(X, Y) UNITTEST_EXPECT(X, Y, ==)
#define UNITTEST_EXPECT_NOTEQUAL(X, Y) UNITTEST_EXPECT(X, Y, !=)
#define UNITTEST_EXPECT_LESSTHAN(X, Y) UNITTEST_EXPECT(X, Y, <)
#define UNITTEST_EXPECT_LESSOREQUAL(X, Y) UNITTEST_EXPECT(X, Y, <=)
#define UNITTEST_EXPECT_GREATERTHAN(X, Y) UNITTEST_EXPECT(X, Y, >)
#define UNITTEST_EXPECT_GREATEROREQUAL(X, Y) UNITTEST_EXPECT(X, Y, =>)

#define UNITTEST_INVALID_PATH(Message) \
    UNITTEST_PRINTF("%s:%u: Failed\n", __FILE__, __LINE__); \
    UNITTEST_PRINTF("Invalid path was taken in the code with the following message: \n"); \
    UNITTEST_PRINTF(Message); \
    *Result = 1;

#endif // HYPERTEST_ASSERTS_H