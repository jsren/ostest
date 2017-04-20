# ostest (C) 2017 James S Renwick
Operating System Unit Test Framework

This utility and all associated source code remains the property of James S Renwick.
Commercial use is forbidden except where permission is explicitly granted.

## Overview ##
ostest is a relatively lightweight C++ unit testing framework similar to Google Test.

The framework has been designed such that it has absolutely no dependencies - not even on
a standard library\*, nor does it in fact require any memory allocation whatsoever\*\*.

\* Except a definition of `size_t` and something to call the static initializers

\*\* Except where your compiler decides to emit it anyway. Set the OSTEST\_NO\_ALLOC preprocessor flag to disable allocation.

## Features ##
 * Unit tests
 * Test suites
 * Create tests in any source file, run from any source file
 * Automatic test discovery/registration
 * Unit test metadata (name, status, assertions) accessible from within test body
 * Assert & Expect statements
 * Comprehensive test results available programatically
 * Results broken down by individual assertions for each test
 * Standard library exception support!
 * Simple, clean syntax
 * Cross-platform C++11, builds with GCC, Clang and MSVC
 * Run/filter specific tests
 * and more...

## Building ##

The ostest library comprises all source files except _example.cpp_ which is intended as a usage example 
and can be built as a standalone executable.

### Preprocessor Flags ###
The following preprocessor flags may be set when building your application:
 * Define `OSTEST_NO_ALLOC` to prevent ostest from allocating memory
 * Define `OSTEST_MUST_PREFIX` to only define the prefixed macros (e.g. `OSTEST_TEST` instead of `TEST`)
 * Define `OSTEST_STD_EXCEPTIONS` to enable C++ exception handling

### With make ###
To build the library, run `make`.
To build the example code, run `make example`.

### With Visual Studio ###
Include all .h and .cpp files in a project, build and run.

## Example ##
```
#include "ostest.h"

TEST_SUITE(ArithmeticSuite)

// Unit tests are created with the TEST macro
TEST(ArithmeticSuite, AdditionTest)
{
    // EXPECT statements log a condition and continue
    EXPECT(1 + 1 == 2);
    EXPECT_EQ(1 + 1, 2);
    EXPECT_NONZERO(1 - 1);

    // ASSERT statements log a condition and end the test if false
    ASSERT(1 - 1 == 0);
    ASSERT_ZERO(-1 + 1);

    // Get test info
    const auto& test = this->getInfo();
    printf("Running test %s::%s (%s:%d)\n", test.suiteName, 
        test.testName, test.file, test.line);

    // Get current result info
    const auto& result = this->getResult();
    printf("Current result: %s\n", result.succeeded() ? "PASSING" : "FAILING");

    const auto* fail = result.getFinalFailure();
    if (fail) printf("Failed: '%s' (%s:%d)\n\n", fail->expression, fail->file, fail->line);
}

// Then to run:
auto tests = ostest::getUnitTests();

while (tests.next()) {
    TestRunner(tests.current()).run();
}
```
