#define WIN32_LEAN_AND_MEAN

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "windows.h"
#include "winsock2.h"
#include "ws2tcpip.h"
#include "win32_hypertest.h"
#include "hypertest_test.h"
#include "hypertest_types.h"
#include "hypertest_asserts.h"
#include "hypertest_http.cpp"

// TODO: Clean the code.
// TODO: Create function for colored output to increase readability.
// TODO: Add further samples to the testing tool. Perhaps look what other
//       tools support.
// TODO: Update this software once additional functionality is needed.

// NOTE: Global Unittest state that will be executed.
struct test_state TestState;

int 
RunTesting(int argc, char ** argv)
{
    LARGE_INTEGER PerfCounterFrequency = {};
    u64 Failed = 0;
    size_t *FailedTestCases = 0;
    size_t NumberOfFailedTestCases = 0;
    u64 RanTests = 0;

    QueryPerformanceFrequency(&PerfCounterFrequency);

    // NOTE: Retrieving console console output info in order to change color of output.
    HANDLE HConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
    WORD SavedAttributes;

    // NOTE: Save current console attributes to reset to once done with colored output.
    GetConsoleScreenBufferInfo(HConsole, &ConsoleInfo);
    SavedAttributes = ConsoleInfo.wAttributes;


    // TODO: Check command line args for --help etc..

    for (u32 Index = 0; Index < TestState.NumberOfTests; ++Index)
    {
        RanTests++;
    }

    printf("    __  __                     ______          __ \n");
    printf("   / / / /_  ______  ___  ____/_  __/__  _____/ /_\n");
    printf("  / /_/ / / / / __ \\/ _ \\/ ___// / / _ \\/ ___/ __/\n");
    printf(" / __  / /_/ / /_/ /  __/ /   / / /  __(__  ) /_  \n");
    printf("/_/ /_/\\__, / .___/\\___/_/   /_/  \\___/____/\\__/  \n");
    printf("      /____/_/                                    \n");
    printf("Created by Oskar Mendel.\n\n");

    SetConsoleTextAttribute(HConsole, FOREGROUND_GREEN);
    printf("[==========]");
    SetConsoleTextAttribute(HConsole, SavedAttributes);
    printf(" Running %I64u tests.\n", (u64) RanTests);
    
    // NOTE: Run tests.
    for (u64 Index = 0; Index < TestState.NumberOfTests; Index++)
    {
        int Result = 0;
        s64 NanoSeconds = 0;
        LARGE_INTEGER PerfCounterStart = {};
        LARGE_INTEGER PerfCounterEnd = {};

        SetConsoleTextAttribute(HConsole, FOREGROUND_GREEN);
        printf("[ RUN      ]");
        SetConsoleTextAttribute(HConsole, SavedAttributes);
        printf(" %s\n", TestState.Tests[Index].Name);
        
        // TODO: If we want file output then do something here..

        QueryPerformanceCounter(&PerfCounterStart);
        TestState.Tests[Index].Function(&Result, TestState.Tests[Index].Index);
        QueryPerformanceCounter(&PerfCounterEnd);
        NanoSeconds = (((PerfCounterEnd.QuadPart - PerfCounterStart.QuadPart) 
            * 1000000000) / PerfCounterFrequency.QuadPart);

        if (Result != 0)
        {
            size_t FailedTestCaseIndex = NumberOfFailedTestCases++;
            FailedTestCases = (size_t *) realloc((void *)FailedTestCases, sizeof(size_t)*NumberOfFailedTestCases);
            FailedTestCases[FailedTestCaseIndex] = (size_t)Index;
            Failed++;

            SetConsoleTextAttribute(HConsole, FOREGROUND_RED);
            printf("[  FAILED  ]");
            SetConsoleTextAttribute(HConsole, SavedAttributes);
            printf(" %s (%I64u ns)\n", TestState.Tests[Index].Name, NanoSeconds);
        }
        else
        {
            SetConsoleTextAttribute(HConsole, FOREGROUND_GREEN);
            printf("[       OK ]");
            SetConsoleTextAttribute(HConsole, SavedAttributes);
            printf(" %s (%I64u ns)\n", TestState.Tests[Index].Name, NanoSeconds);
        }
    }

    // NOTE: Printing overview data of entire ran test.
    SetConsoleTextAttribute(HConsole, FOREGROUND_GREEN);
    printf("[==========]");
    SetConsoleTextAttribute(HConsole, SavedAttributes);
    printf(" %I64u test cases ran.\n", RanTests);

    SetConsoleTextAttribute(HConsole, FOREGROUND_GREEN);
    printf("[  PASSED  ]");
    SetConsoleTextAttribute(HConsole, SavedAttributes);
    printf(" %I64u tests.\n", RanTests - Failed);

    if (Failed)
    {
        SetConsoleTextAttribute(HConsole, FOREGROUND_RED);
        printf("[  FAILED  ]");
        SetConsoleTextAttribute(HConsole, SavedAttributes);
        printf(" %I64u tests, listed bellow:\n", Failed);

        // TODO: For loop and loop through failed tests.
        for (u64 Index = 0; Index < NumberOfFailedTestCases; ++Index)
        {
            SetConsoleTextAttribute(HConsole, FOREGROUND_RED);
            printf("[  FAILED  ]");
            SetConsoleTextAttribute(HConsole, SavedAttributes);
            printf(" %s\n", TestState.Tests[FailedTestCases[Index]].Name);
        }
    }

    // NOTE: Cleanup of allocated test data
    for (u64 Index = 0; Index < TestState.NumberOfTests; ++Index)
    {
        free((void *) TestState.Tests[Index].Name);
    }

    free((void *) TestState.Tests);

    return ((s32) Failed);
}

int 
main(int argc, char **argv)
{
    return RunTesting(argc, argv);
}