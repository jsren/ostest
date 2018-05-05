
# ostest (C) 2018 James Renwick
Operating System Unit Test Framework

This utility and all associated source code remains the property of James S Renwick.
Commercial use is forbidden except where permission is explicitly granted.

![Build Status Badge](https://jsren.github.io/test-results/Build%20OSTest/build-status.svg)
![Test Status Badge](https://jsren.github.io/test-results/Build%20OSTest/test-status.svg)


## Overview ##
ostest is a relatively lightweight C++ unit testing framework similar to Google Test.

The framework has been designed such that it has absolutely no dependencies - not even on
a standard library\*, nor does it in fact require any memory allocation whatsoever\*\*.

\* Except the basic ABI functions to call object initialisers

\*\* Except where your compiler decides to emit it anyway.

## Features ##
 * Unit tests
 * Test suites with overridable setup/tear-down
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
 * Custom per-test metadata
 * and more...

## Building ##

The ostest library comprises all source files in the root dierctory except _example.cpp_ which is intended as a usage example
and can be built as a standalone executable.

An additional _selftest_ directory contains test files for testing the ostest library with itself.

Profiles are sets of additional compiler flags designed to meet particular usage scenarios, such as
barebones/OS or full user-mode applications.

### Preprocessor Flags ###
The following preprocessor flags may be set when building the ostest library:
 * Define `OSTEST_NO_ALLOC` to prevent ostest from allocating memory
 * Define `OSTEST_STD_EXCEPTIONS` to enable C++ exception handling

The following preprocessor flags may be set when including the ostest headers:
 * Define `OSTEST_MUST_PREFIX` to only define the prefixed macros (e.g. `OSTEST_TEST` instead of `TEST`)

### With make ###
To build the library, run `make`.
To build the example code, run `make example`.
To build ostest's ostest tests, run `make test`.
To build all, run `make all`.

Target profiles can be specified with `PROFILE=`, as can the C++ compiler with `CXX=`.
Available profiles can be found under the _profiles_ directory.

#### Example ####
`make all CXX=clang++ PROFILE=bare`

### With Visual Studio ###
Include all .hpp and .cpp files in a project, build and run.

## Example ##
```c++
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
    printf("Running test %s::%s (%s:%d)\n", test.suite.name,
        test.name, test.file, test.line);

    // Get current result info
    const auto& result = this->getResult();
    printf("Current result: %s\n", result.succeeded() ? "PASSING" : "FAILING");

    const auto* fail = result.getFinalFailure();
    if (fail) printf("Failed: '%s' (%s:%d)\n\n", fail->expression, fail->file, fail->line);
}

// Then to run:
for (SuiteInfo& suiteInfo : ostest::getSuites())
{
    auto suite = suiteInfo.getSingletonSmartPtr();
    for (auto& test : suiteInfo.tests()) {
        auto result = TestRunner(*suite, test).run();
    }
}
```
