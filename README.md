# ostest (C) 2017 James S Renwick
Operating System Unit Test Framework

This utility and all associated source code remains the property of James S Renwick.
Commercial use is forbidden except where permission is explicitly granted.

## Overview ##
ostest is a relatively lightweight C++ unit testing framework based on Google Test.

The framework has been designed such that it has absolutely no dependencies - not even on
a particular standard library\*, nor does it in fact require any memory allocation whatsoever\*\*.



\* Except a definition of `size_t`.
\*\* Except where your compiler decides to emit it anyway. Set the OSTEST_NO_ALLOC preprocessor flag to disable allocation.

## Features ##
 * Named unit tests
 * Test suites
 * Test suite setUp/tearDown for each test instance
 * Unit test metadata accessible from within tests
 * Basic assertions and GTest-style expectations
 * Comprehensive test results available programatically
 * Assertion result break-down for each test instance
 * Standard library exception support!
 * Test-complete callback
 * Simple, clean syntax
 * Cross-platform C++11
 * Run/filter specific tests
 * and more...

## Building ##

### With make ###
To build the library, run `make`.
To build the example code, run `make example`.

### With Visual Studio ###
Include all .h and .cpp files in a project, build and run.

The ostest library comprises all files except _example.cpp_
