/* common.cpp - (c) 2017 James S Renwick */
#include "common.hpp"
#include <ostest.h>
#include <cstring>
#include <stdio.h>

using namespace ostest;

static bool string_ends_with(const std::string& string, const std::string& suffix)
{
    return string.size() >= suffix.size() &&
        string.compare(string.size()-suffix.size(), suffix.size(), suffix) == 0;
}


bool testShouldFail(const TestInfo& test)
{
    return string_ends_with(test.testName, "Fail");
}

void printTestResult(const TestInfo& test, bool succeeded, const TestResult& result)
{
    static const char* passStr = "PASS";
    static const char* failStr = "FAIL";

    if (succeeded)
    {
        // Print test result
        printf("[PASS] [%s::%s] at %s:%i\n", test.suiteName,
            test.testName, test.file, test.line);
    }
    else
    {
        // Print test result
        printf("[FAIL] [%s::%s] unexpected %s at %s:%i\n", test.suiteName,
            test.testName, result ? passStr : failStr, test.file, test.line);
    }
}


int main()
{
    auto tests = ostest::getUnitTests();
    while (tests.next())
    {
        auto& test = tests.current();
        // Skip internal tests
        if (std::strlen(test.suiteName) > 0 && test.suiteName[0] == '_') continue;
        // Run normal tests
        TestRunner(test).run();
    }
    return 0;
}

void ostest::handleTestComplete(const TestInfo&, const TestResult&)
{

}
