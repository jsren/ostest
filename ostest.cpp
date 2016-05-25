/* ostest.cpp - (c) James S Renwick 2016 */
#include "ostest.h"

namespace ostest
{
    // Tests for string equality
    bool streq(const char* s1, const char* s2)
    {
        for (unsigned long int i = 0; ; i++)
        {
            if (s1[i] != s2[i]) return false;
            else if (s1[i] == s2[i] && s1[i] == '\0') return true;
        }
        return false;
    }

    // Creates a new assertion
    Assertion::Assertion(const char* file, int line)
        : result(true), message(nullptr), file(file), line(line) { }

    bool Assertion::assert(UnitTest& test, bool result)
    {
        // Reset state in case values already set by previous invocation
        this->result   = result;
        this->nextItem = nullptr;

        // Update linked list
        if (test.result.firstItem == nullptr) test.result.firstItem = this;

        if (test.result.finalItem != nullptr && test.result.finalItem != this) 
            test.result.finalItem->nextItem = this;

        test.result.finalItem = this;

        return result;
    }

    bool AssertionEnumerator::next()
    {
        // Handle first element
        if (initial) {
            initial = false;
        }
        else if (item != nullptr) {
            item = item->nextItem;
        }
        return item != nullptr;
    }

    bool TestResult::succeeded() const
    {
        AssertionEnumerator assertions(this->firstItem);

        while (assertions.next()) {
            if (!assertions.current()->passed()) return false;
        }
        return true;
    }

    const Assertion* TestResult::getFirstFailure() const
    {
        AssertionEnumerator assertions(this->firstItem);

        while (assertions.next()) {
            if (!assertions.current()->passed()) return assertions.current();
        }
        return nullptr;
    }

    const Assertion* TestResult::getFinalFailure() const
    {
        AssertionEnumerator assertions(this->firstItem);

        const Assertion* final = nullptr;

        while (assertions.next()) {
            if (!assertions.current()->passed()) final = assertions.current();
        }
        return final;
    }

    /* Creates and registers a new unit test with the given details.
       Returns the new test's test info.
    */
    const TestInfo TestInfo::registerNew(const char* suiteName, const char* testName,
        ITestWrapper& wrapper, const char* file, int line)
    {
        itemCount++;
        return TestInfo(suiteName, testName, wrapper, file, line);
    }

    TestInfo::TestInfo(const char* suiteName, const char* testName,
        ITestWrapper& wrapper, const char* file, int line) 
        : wrapper(wrapper), line(line), file(file),
        suiteName(suiteName), testName(testName)
    {
        // Update linked list
        if (firstItem == nullptr) firstItem = this;
        if (finalItem != nullptr) finalItem->nextItem = this;
        finalItem = this;
    }


    bool TestEnumerator::next()
    {
        // Handle first element
        if (initial) {
            item = TestInfo::firstItem; initial = false;
        }
        else if (item != nullptr) {
            item = item->nextItem;
        }

        // Handle suite filter
        if (suite != nullptr)
        {
            while (item != nullptr)
            {
                if (streq(suite, item->suiteName)) return true;
                else item = item->nextItem;
            }
        }

        return item != nullptr;
    }
    

    TestResult TestRunner::run()
    {
        // Get the test and suite instances
        UnitTest& test = info.wrapper.getTest();
        TestSuite& suite = info.wrapper.getSuite();

        // Perform testing
        suite.setUp();
        test.testBody();
        suite.tearDown();

        // Clean up
        auto result = test.result;
        info.wrapper.deleteInstance();

        // Notify test complete
        ::ostest::handleTestComplete(info, result);
        return result;
    }

}
