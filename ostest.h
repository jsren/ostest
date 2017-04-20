/* ostest.h - (c) 2017 James S Renwick */
#pragma once

#define OSTEST_VERSION 1   // Major version number of ostest
#define OSTEST_REVISION 2  // Revision number of ostest

/* Flag switching whether OSTEST can use 'new/delete'.
   When set, this must also be set when building ostest 
   implementation files. */
#ifndef OSTEST_NO_ALLOC
#define OSTEST_NO_ALLOC 0
#endif

/* Flag switching whether OSTEST can define 'TEST' etc.
   without the OSTEST_ prefix. */
#ifndef OSTEST_MUST_PREFIX
#define OSTEST_MUST_PREFIX 0
#endif

/* Flag switching whether OSTEST handles C++ standard 
   library exceptions. */
#ifndef OSTEST_STD_EXCEPTIONS
#define OSTEST_STD_EXCEPTIONS 0
#endif

/* */
#ifdef assert
#error "C assert macro not compatible with this library. Please undefine."
#endif

#include "ostest-impl.h"
#include "ostest-assert.h"


namespace ostest
{
    /* User-defined test-complete handler. Run once a test has completed. */
    void handleTestComplete(const ostest::TestInfo&, 
        const ostest::TestResult&) noexcept;

    /* Returns an enumerator over the defined unit tests. */
    inline TestEnumerator getUnitTests() noexcept {
        return TestEnumerator();
    }

    /* Returns an enumerator over the unit tests defined in the given suite. */
    inline TestEnumerator getUnitTests(const char* suiteName) noexcept {
        return TestEnumerator(suiteName);
    }
}
