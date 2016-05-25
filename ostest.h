/* ostest.h - (c) James S Renwick 2016 */
#include "ostest-impl.h"
#include "ostest-assert.h"

namespace ostest
{
    // This must be provided by users
    /* Method called when a test has completed execution. */
    extern void handleTestComplete(const ostest::TestInfo& test,
        const ostest::TestResult& result) noexcept;

    /* Returns an enumerator over the defined unit tests. */
    inline TestEnumerator getUnitTests() noexcept {
        return TestEnumerator();
    }

    /* Returns an enumerator over the unit tests defined in the given suite. */
    inline TestEnumerator getUnitTests(const char* suiteName) noexcept {
        return TestEnumerator(suiteName);
    }
}
