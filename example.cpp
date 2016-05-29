/* example.cpp - (c) James S Renwick 2016 */

#define OSTEST_HANDLE_TEST_COMPLETE 1

#include <cstdio>
#include "ostest.h"

using namespace ostest;


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
        const Assertion* assert = assertions.current();

        if (assert == first && assert == final) printf("  ONLY: ");
        else if (assert == first) printf(" FIRST: ");
        else if (assert == final) printf(" FINAL: ");
        else printf("\t");

        printf("[%s] [%s:%i] \"%s\"\n", assert->passed() ? passStr : failStr, assert->file, 
		    assert->line, assert->expression);
    }

    // Newline separator
    printf("\n");
}



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



TEST(CustomSuite, TestWithWhileLoop)
{
    while (this->testInt > 0)
    {
        EXPECT_NONZERO(this->testInt);
		
		// This will fail to build when OSTEST_NO_ALLOC defined
		EXPECT_ALL(this->testInt % 2 == 0);

        this->testInt--;
    }
    ASSERT_NONZERO(this->testInt);
}

TEST(CustomSuite, EmptyTest)
{
    
}


namespace example
{
    OSTEST_TEST_SUITE(CustomSuite)
}

TEST_EX(::example, CustomSuite, ScopedTest)
{
    EXPECT(true);
    EXPECT(false);
    EXPECT(true);
    EXPECT(false);
    EXPECT(true);
}


int main()
{
    auto tests = ostest::getUnitTests();

    while (tests.next()) {
        TestRunner(*tests.current()).run();
    }
    return 0;
}
