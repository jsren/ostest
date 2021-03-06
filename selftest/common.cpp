/* common.cpp - (c) 2018 James Renwick */
#include "common.hpp"
#include <ostest.hpp>
#include <cstring>
#include <stdio.h>

using namespace ostest;


size_t countAssertions(const TestResult& result)
{
    size_t assertionCount = 0;
    for (auto& _: result.getAssertions()) {
        (void)_;
        assertionCount++;
    }
    return assertionCount;
}

TestExpect getTestFailRequirement(const TestInfo& test)
{
    auto* meta = test.getMetadata<TestExpect>("expect");
    return meta ? meta->value : TestExpect::AllPass;
}

bool assertionShouldFail(const TestInfo& test, const Assertion& target)
{
    auto* fails = test.getMetadata<Assertion**>("fails");
    if (!fails) return false;

    for (Assertion** stmt = fails->value; *stmt != nullptr; stmt++) {
        if (*stmt == &target) return true;
    }
    return false;
}

bool testPassed(const TestInfo& test, const TestResult& result)
{
    auto req = getTestFailRequirement(test);

    if (req == TestExpect::AllPass) {
        return result.succeeded();
    }
    else if (req == TestExpect::AllFail) {
        return !result.succeeded() && allAssertionsFailed(result);
    }
    else if (req == TestExpect::SomeFail)
    {
        for (auto& assertion : result.getAssertions())
        {
            bool shouldFail = assertionShouldFail(test, assertion);
            if (assertion.passed() != !shouldFail) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool allAssertionsFailed(const TestResult& result)
{
    for (auto& assert : result.getAssertions()) {
        if (assert.passed()) return false;
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
        printf("[PASS] [%s::%s] at %s:%i\n", test.suite.name,
            test.name, test.file, test.line);
    }
    else
    {
        // Print test result
        printf("[FAIL] [%s::%s] unexpected %s at %s:%i\n", test.suite.name,
            test.name, result ? passStr : failStr, test.file, test.line);

        for (auto& assert : result.getAssertions()) {
            printf("\t\"%s\" at %s:%i\n", assert.getMessage(), assert.file, assert.line);
        }
    }
}


int main()
{
    for (SuiteInfo& suiteInfo : ostest::getSuites())
    {
        auto suite = suiteInfo.getSingletonSmartPtr();
        for (auto& test : suiteInfo.tests())
        {
            // Skip internal tests
            if (std::strlen(test.suite.name) > 0 && test.suite.name[0] == '_') continue;
            // Run normal tests
            TestRunner(*suite, test).run();
        }
    }
    return 0;
}

void ostest::handleTestComplete(const TestInfo&, const TestResult&)
{

}
