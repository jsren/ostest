/* common.cpp - (c) 2017 James S Renwick */
#include "common.hpp"
#include <ostest.h>
#include <cstring>
#include <stdio.h>

using namespace ostest;


size_t countAssertions(const TestResult& result)
{
    auto enum_ = result.getAssertions();
    size_t assertionCount = 0;
    while (enum_.next()) assertionCount++;
    return assertionCount;
}

bool testShouldFail(const TestInfo& test)
{
    auto* meta = test.getMetadata<TestExpect>("expect");
    return meta && meta->value == TestExpect::AllFail;
}

bool allAssertionsFailed(const TestResult& result)
{
    auto enum_ = result.getAssertions();
    while (enum_.next()) {
        if (enum_.current().passed()) return false;
    }
    return true;
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

        auto enum_ = result.getAssertions();
        while (enum_.next())
        {
            auto& assertion = enum_.current();
            printf("\t\"%s\" at %s:%i\n", assertion.getMessage(), assertion.file, assertion.line);
        }
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
