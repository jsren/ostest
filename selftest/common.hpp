/* common.hpp - (c) 2017 James S Renwick */
#include <ostest.h>
#include <stddef.h>
#include <string>

enum class TestExpect
{
    AllPass,
    AllFail
};

size_t countAssertions(const ostest::TestResult& result);

bool testShouldFail(const ostest::TestInfo& test);

bool allAssertionsFailed(const ostest::TestResult& result);

void printTestResult(const ostest::TestInfo& test, bool succeeded,
    const ostest::TestResult& result);
