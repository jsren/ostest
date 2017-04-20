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
    int testInt = 0;

    CustomSuite() {
        //printf("[INFO] [CustomSuite] Constructor Called.\n");
    }

    virtual void setUp() override final
    {
        testInt = 4; // Reset value before each test
        //printf("[INFO] [CustomSuite] SetUp Called.\n");
    }

    virtual void tearDown() override final {
        //printf("[INFO] [CustomSuite] Tear Down Called.\n");
    }

    ~CustomSuite() {
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

TEST(CustomSuite, TestWithLoop)
{
    ASSERT_NONZERO(this->testInt);
    
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
    ASSERT_ZERO(this->testInt);
}


TEST(CustomSuite, TestWithLoopBreak)
{
    ASSERT_NONZERO(this->testInt);

    while (this->testInt > 0)
    {
        // EXPECT/ASSERT statements only log the condition from the final loop iteration 
        EXPECT_NONZERO(this->testInt);

        // EXPECT(_ALL)_OR_BREAK will break if the expression is false
        EXPECT_OR_BREAK(this->testInt % 2 == 0);

        // This is just syntactic sugar for the below:
        bool expr = this->testInt % 2 == 0;
        EXPECT(expr); if (!expr) break;

        this->testInt--;
    }
    ASSERT_ZERO(this->testInt);
}


// Unhandled exceptions during tests are logged and treated as a failed ASSERT
#include <stdexcept>
TEST(CustomSuite, ExceptionTest)
{
    // Only throw if supported
    if (ostest::ostest_std_exceptions) {
        throw std::runtime_error("This is a test exception.");
    }
}



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
    printf("Built with: %s %s\n", 
        ostest::ostest_no_alloc ? "OSTEST_NO_ALLOC" : "",
        ostest::ostest_std_exceptions ? "OSTEST_STD_EXCEPTIONS" : "");
    printf("\n");

    // Gets all available unit tests
    auto tests = ostest::getUnitTests();

    // Creates a test runner to run the test.
    // Results are reported in 'handleTestComplete.
    while (tests.next()) {
        auto result = TestRunner(tests.current()).run();
    }
    return 0;
}


// Called when a test has completed.
void ostest::handleTestComplete(const TestInfo& test, const TestResult& result)
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

        if (ostest::ostest_std_exceptions) {
            if (strcmp(assert->expression, "<unhandled exception>") == 0) {
                printf("\t   %s\n", assert->getMessage());
            }
        }
    }

    // Newline separator
    printf("\n");
}
