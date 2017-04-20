/* ostest.cpp - (c) 2017 James S Renwick */
#include "ostest-impl.h"
#include "ostest-exception.h"

namespace ostest
{
    // Abstract base class
    TestSuite::~TestSuite() { }


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
    Assertion::Assertion(const char* expr, const char* file, int line, bool tmp) : result(true), message(nullptr), 
        nextItem(nullptr), prevItem(nullptr), temporary(tmp), expression(expr), file(file), line(line) { }

    bool Assertion::evaluate(UnitTest& test, bool result)
    {
        // Reset state in case values already set by previous invocation.
        // (Linked list state should have been reset by TestResult destructor.)
        this->result = result;

        // This function always inserts the assertion at the end of the list.
        // Thus if already present in list, update existing list pointers.
        if (this->nextItem != nullptr) this->nextItem->prevItem = this->prevItem;
        if (this->prevItem != nullptr) this->prevItem->nextItem = this->nextItem;

        // Update first item
        if (this->prevItem == nullptr && this->nextItem != nullptr) {
            test.result.firstItem = this->nextItem; 
        }

        // Add to end of list
        this->prevItem = nullptr;
        this->nextItem = nullptr;

        // Update list first and final pointers
        if (test.result.firstItem == nullptr) test.result.firstItem = this;
        if (test.result.finalItem != nullptr && test.result.finalItem != this) 
        {
            this->prevItem = test.result.finalItem;
            test.result.finalItem->nextItem = this;
        }
        test.result.finalItem = this;

        return result;
    }

    bool AssertionEnumerator::next()
    {
        // Handle first element
        if (initial) 
        {
            initial = false; 
            return item != nullptr;
        }
        else if (item != nullptr && item->nextItem != nullptr)
        {
            item = item->nextItem;
            return true;
        }
        else return false;
    }

    bool AssertionEnumerator::previous()
    {
        if (item != nullptr && item->prevItem)
        {
            item = item->prevItem;
            return true;
        }
        else return false;
    }

#if !OSTEST_NO_ALLOC
    TestResult::TestResult() : refCount(new unsigned int(1))
    {

    }

    TestResult::TestResult(const TestResult& copy)
    {
        this->firstItem = copy.firstItem;
        this->finalItem = copy.finalItem;
        this->itemCount = copy.itemCount;
        this->refCount  = copy.refCount;

        (*refCount)++;
    }

    TestResult::~TestResult()
    {
        // This is the case upon move
        if (this->refCount != nullptr)
        {
            this->refCount[0]--;

            if (*this->refCount == 0)
            {
                Assertion *next = this->firstItem;
                while (next != nullptr)
                {
                    Assertion* tmp = next->nextItem;

                    // Delete or reset 
                    if (next->temporary) delete next;
                    else next->prevItem = next->nextItem = nullptr;

                    next = tmp; // Return next assertion
                }
            }
        }
    }
#endif

    bool TestResult::succeeded() const
    {
        AssertionEnumerator assertions(this->firstItem);

        while (assertions.next()) {
            if (!assertions.current().passed()) return false;
        }
        return true;
    }

    const Assertion* TestResult::getFirstFailure() const
    {
        AssertionEnumerator assertions(this->firstItem);

        while (assertions.next()) {
            if (!assertions.current().passed()) return &assertions.current();
        }
        return nullptr;
    }

    const Assertion* TestResult::getFinalFailure() const
    {
        AssertionEnumerator assertions(this->firstItem);

        const Assertion* final = nullptr;

        while (assertions.next()) {
            if (!assertions.current().passed()) final = &assertions.current();
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

#if OSTEST_STD_EXCEPTIONS
        try {
            test.testBody();
        }
        catch (const std::exception& e) {
            (new NoExceptionAssertion(e, test.getInfo()))->evaluate(test, false);
        }
        // Please do not use 'new' when throwing exceptions!
        // This will NOT free the exception. This is to avoid aborting later.
        catch (const std::exception* e) {
            (new NoExceptionAssertion(*e, test.getInfo()))->evaluate(test, false);
        }
        catch (const std::string& str) {
            (new NoExceptionAssertion(std::runtime_error(str.c_str()), 
                test.getInfo()))->evaluate(test, false);
        }
        catch (const char* msg) {
            (new NoExceptionAssertion(std::runtime_error(msg), test.getInfo()))->evaluate(test, false);
        }
        catch (...) {
            (new NoExceptionAssertion(std::exception(), test.getInfo()))->evaluate(test, false);
        }
#else
        test.testBody();
#endif
        suite.tearDown();

        // Clean up
        auto result = test.result;
        info.wrapper.deleteInstance();

        // Notify test complete
        ::ostest::handleTestComplete(info, result);
        return result;
    }


#if OSTEST_STD_EXCEPTIONS

    static const char noexceptionMsg[] = "An unhandled exception occurred: ";

    NoExceptionAssertion::NoExceptionAssertion(const std::exception& exception, 
        const TestInfo& test, bool temp) : Assertion("<unhandled exception>", 
            test.file, test.line, temp)
    {
        auto what = exception.what();

        // Get exception message string length (up to 816)
        unsigned long length = 0;
        for (; length <= 816; length++) {
            if (what[length] == '\0') break;
        }

        unsigned long msgIndex = 0;
        auto msg = new char[sizeof(noexceptionMsg) + length];

        // Copy message prefix
        for (; msgIndex < sizeof(noexceptionMsg) - 1; msgIndex++) {
            msg[msgIndex] = noexceptionMsg[msgIndex];
        }
        // Copy exception message
        for (unsigned long i = 0; i < length + 1; i++, msgIndex++) {
            msg[msgIndex] = what[i];
        }
        this->exceptionMsg = msg;
    }

    NoExceptionAssertion::~NoExceptionAssertion() {
        if (this->exceptionMsg != nullptr) delete[] this->exceptionMsg;
    }

    NoExceptionAssertion::NoExceptionAssertion(NoExceptionAssertion&& other)
        : Assertion(other.expression, other.file, other.line, other.temporary)
    {
        this->exceptionMsg = other.exceptionMsg;
        other.exceptionMsg = nullptr;
    }

    const char* NoExceptionAssertion::getMessage() const
    {
        return this->exceptionMsg;
    }

#endif
    

}
