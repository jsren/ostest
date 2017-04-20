/* example.cpp - (c) 2017 James S Renwick */
#include "ostest.h"
#include <cstdio>
#include <cstring>

using namespace ostest;


// Each unit test belongs to a test suite
// Default test suites are created via a macro:
TEST_SUITE(EmptySuite)

// ...or can be created manually to enable overriding behaviour:
class CustomSuite : public ::ostest::TestSuite
{
protected:
    int testInt;

    CustomSuite() : testInt(4)
    {
        //printf("[INFO] [CustomSuite] Constructor Called.\n");
    }

    void setUp() override
    {
        //printf("[INFO] [CustomSuite] SetUp Called.\n");
    }

    void tearDown() override
    {
        //printf("[INFO] [CustomSuite] Tear Down Called.\n");
    }

    virtual ~CustomSuite()
    {
        //printf("[INFO] [CustomSuite] Destructor Called.\n");
    }
};


// Unit tests are created with the TEST macro
TEST(EmptySuite, EmptyTest)
{
    // EXPECT statements log a condition and continue
    EXPECT(1 == 1);
    EXPECT_EQ(1, 1);

    // ASSERT statements log a condition and end the test if false
    ASSERT(1 != 2);
    ASSERT_NEQ(1, 2);
}

TEST(CustomSuite, TestWithWhileLoop)
{
    while (this->testInt > 0)
    {
        // EXPECT/ASSERT statements only log the condition from the final loop iteration 
        EXPECT_NONZERO(this->testInt);
        
#if !OSTEST_NO_ALLOC
        // EXPECT_ALL/ASSERT_ALL will log the condition for all loop iterations.
        // It requires memory allocation.
        EXPECT_ALL(this->testInt % 2 == 0);
#endif
        this->testInt--;
    }
    ASSERT_NONZERO(this->testInt);
}



#if OSTEST_STD_EXCEPTIONS
#include <stdexcept>

// Unhandled exceptions during tests are logged and treated as a failed ASSERT
TEST(CustomSuite, ExceptionTest)
{
    throw std::runtime_error("This is a test exception.");
}
#endif



// Test suites can be declared within namespaces...
namespace exampleNS
{
    TEST_SUITE(CustomSuite)
}
// ...but tests for these must be defined with TEST_EX
TEST_EX(::exampleNS, CustomSuite, ScopedTest)
{
    EXPECT(true);
    EXPECT(false);
    EXPECT(true);
    EXPECT(false);
    EXPECT(true);
}



int main()
{
    // Print application header
    printf("\n");
    printf("ostest example application\n");
    printf("--------------------------------------\n");
    printf("Version %d.%d (c) 2017 James S Renwick\n", OSTEST_VERSION, OSTEST_REVISION);
    printf("\n");

    // Gets all available unit tests
    auto tests = ostest::getUnitTests();

    // Creates a test runner to run the test.
    // Results are reported in 'handleTestComplete.
    while (tests.next()) {
        TestRunner(tests.current()).run();
    }
    return 0;
}


// Called when a test has completed.
void ostest::handleTestComplete(const TestInfo& test, const TestResult& result) noexcept
{
    static const char* passStr = "PASS";
    static const char* failStr = "FAIL";

    // Print test result
    printf("[%s] [%s::%s] at %s:%i\n", result ? passStr : failStr, test.suiteName,
        test.testName, test.file, test.line);


    auto first = result.getFirstFailure();
    auto final = result.getFinalFailure();

    // Log test assertions & their results
    auto assertions = result.getAssertions();
    while (assertions.next())
    {
        const Assertion* assert = &assertions.current();

        if (assert == first && assert == final) printf("  ONLY: ");
        else if (assert == first) printf(" FIRST: ");
        else if (assert == final) printf(" FINAL: ");
        else printf("\t");

        printf("[%s] [%s:%i] \"%s\"\n", assert->passed() ? passStr : failStr, assert->file, 
            assert->line, assert->expression);

#if OSTEST_STD_EXCEPTIONS
        if (strcmp(assert->expression, "<unhandled exception>") == 0) {
            printf("\t   %s\n", assert->getMessage());
        }
#endif
    }

    // Newline separator
    printf("\n");
}
