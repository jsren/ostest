/* common.hpp - (c) 2017 James S Renwick */
#include <ostest.h>
#include <stddef.h>
#include <string>

bool testShouldFail(const ostest::TestInfo& test);

void printTestResult(const ostest::TestInfo& test, bool succeeded,
    const ostest::TestResult& result);
