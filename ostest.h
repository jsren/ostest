/* ostest.h - (c) 2017 James S Renwick */
#pragma once

#define OSTEST_VERSION 1   // Major version number of ostest
#define OSTEST_REVISION 4  // Revision number of ostest


/* When set, ostest will only include macros prefixed with OSTEST_
   e.g. OSTEST_TEST instead of TEST. Can be set on a per-file basis.
*/
//#define OSTEST_MUST_PREFIX


#include "ostest-impl.h"
#include "ostest-assert.h"

namespace ostest
{
    /* Flag indicating whether ostest can allocate heap memory.
    Set when ostest compiled with 'OSTEST_NO_ALLOC'.
    */
    extern const bool ostest_no_alloc;

    /* Flag switching whether ostest supports C++ standard
    library exceptions. Set when ostest compiled with 'OSTEST_STD_EXCEPTIONS'.
    */
    extern const bool ostest_std_exceptions;


    /* User-defined test-complete handler. Run once a test has completed. */
    void handleTestComplete(const ostest::TestInfo&,
        const ostest::TestResult&);

    /* Returns an enumerator over the defined unit tests. */
    inline TestEnumerator getUnitTests() noexcept {
        return TestEnumerator();
    }

    /* Returns an enumerator over the unit tests defined in the given suite. */
    inline TestEnumerator getUnitTests(const char* suiteName) noexcept {
        return TestEnumerator(suiteName);
    }
}
