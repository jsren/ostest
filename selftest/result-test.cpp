/* result-test.cpp - (c) 2018 James Renwick */
#include "common.hpp"

using namespace ostest;

namespace selftest
{
#define TEST_EXPECT_ALL_FAIL \
    static Metadata<TestExpect> _(*this, "expect", TestExpect::AllFail)

    class _ResultSuite : public TestSuite
    {
    public:
        TestResult emptyResult{};
        TestResult result{};
    };

    TEST_EX(::selftest, _ResultSuite, _ResultCorrect)
    {
        // Test correct initial result
        auto iter1 = getResult().getAssertions();
        EXPECT_EQ(iter1.begin(), iter1.end());
        EXPECT(getResult());
        EXPECT(getResult().succeeded());
        EXPECT_EQ(getResult().getFirstFailure(), nullptr);
        EXPECT_EQ(getResult().getFinalFailure(), nullptr);

        // Add a failed assertion
        EXPECT(false);
        ASSERT_NEQ(getResult().getFinalFailure(), nullptr);
        const Assertion* fail1 = getResult().getFinalFailure();

        // Test that result now indicates failure
        EXPECT(!getResult());
        EXPECT(!getResult().succeeded());
        EXPECT_NEQ(getResult().getFirstFailure(), nullptr);
        EXPECT_NEQ(getResult().getFinalFailure(), nullptr);
        EXPECT_EQ(getResult().getFirstFailure(), getResult().getFinalFailure());

        // Test assertion iteration
        auto iter2 = getResult().getAssertions();
        {
            auto current = iter2.begin();
            auto end = iter2.end();
            EXPECT_NEQ(current, end);

            // The first and only incorrect assertion is the 6th
            for (uint8_t i = 0; i < 5 && current != end; i++, current++) {
                EXPECT((*current).passed());
            }
            ASSERT_NEQ(current, end);

            EXPECT_EQ(&*current, getResult().getFirstFailure());
            EXPECT_EQ(&*current, getResult().getFinalFailure());
            EXPECT(!(*current).passed());

            current++;
            bool allPassed = true;
            for (uint8_t i = 0; i < 22-6 && current != end; i++, current++) {
                if (!(*current).passed()) { allPassed = false; break; }
            }
            bool atEnd = (current == end);
            EXPECT(atEnd);
            EXPECT(allPassed);
        }

        // Test that result still indicates failure
        EXPECT(!getResult());
        EXPECT(!getResult().succeeded());
        EXPECT_NEQ(getResult().getFirstFailure(), nullptr);
        EXPECT_NEQ(getResult().getFinalFailure(), nullptr);
        EXPECT_EQ(getResult().getFirstFailure(), getResult().getFinalFailure());

        // Add an additional failure
        EXPECT_EQ(1, 2);
        ASSERT_NEQ(getResult().getFinalFailure(), nullptr);
        const Assertion* fail2 = getResult().getFinalFailure();

        // Test that result still indicates failure
        EXPECT(!getResult());
        EXPECT(!getResult().succeeded());
        EXPECT_NEQ(getResult().getFirstFailure(), nullptr);
        EXPECT_NEQ(getResult().getFinalFailure(), nullptr);
        EXPECT_NEQ(getResult().getFirstFailure(), getResult().getFinalFailure());

        // Test copying result object
        TestResult result1{getResult()};
        EXPECT_EQ(result1.succeeded(), getResult().succeeded());
        EXPECT_EQ(result1.getFirstFailure(), getResult().getFirstFailure());
        EXPECT_EQ(result1.getFinalFailure(), getResult().getFinalFailure());
        EXPECT_EQ(result1.getAssertions().begin(), getResult().getAssertions().begin());
        EXPECT_EQ(result1.getAssertions().end(), getResult().getAssertions().end());

        TestResult result2{};
        result2 = getResult();
        EXPECT_EQ(result2.succeeded(), getResult().succeeded());
        EXPECT_EQ(result2.getFirstFailure(), getResult().getFirstFailure());
        EXPECT_EQ(result2.getFinalFailure(), getResult().getFinalFailure());
        EXPECT_EQ(result2.getAssertions().begin(), getResult().getAssertions().begin());
        EXPECT_EQ(result2.getAssertions().end(), getResult().getAssertions().end());

        // Test move constructor
        TestResult emptyResult{};
        TestResult result3{reinterpret_cast<TestResult&&>(result1)};

        // Original result must now be reset
        EXPECT_EQ(result1.succeeded(), emptyResult.succeeded());
        EXPECT_EQ(result1.getFirstFailure(), emptyResult.getFirstFailure());
        EXPECT_EQ(result1.getFinalFailure(), emptyResult.getFinalFailure());
        EXPECT_EQ(result1.getAssertions().begin(), emptyResult.getAssertions().begin());
        EXPECT_EQ(result1.getAssertions().end(), emptyResult.getAssertions().end());

        EXPECT_EQ(result3.succeeded(), result2.succeeded());
        EXPECT_EQ(result3.getFirstFailure(), result2.getFirstFailure());
        EXPECT_EQ(result3.getFinalFailure(), result2.getFinalFailure());
        EXPECT_EQ(result3.getAssertions().begin(), result2.getAssertions().begin());
        EXPECT_EQ(result3.getAssertions().end(), result2.getAssertions().end());

        // Test move assignment
        TestResult result4{};
        result4 = reinterpret_cast<TestResult&&>(result2);

        // Original result must now be reset
        EXPECT_EQ(result2.succeeded(), emptyResult.succeeded());
        EXPECT_EQ(result2.getFirstFailure(), emptyResult.getFirstFailure());
        EXPECT_EQ(result2.getFinalFailure(), emptyResult.getFinalFailure());
        EXPECT_EQ(result2.getAssertions().begin(), emptyResult.getAssertions().begin());
        EXPECT_EQ(result2.getAssertions().end(), emptyResult.getAssertions().end());

        EXPECT_EQ(result4.succeeded(), result3.succeeded());
        EXPECT_EQ(result4.getFirstFailure(), result3.getFirstFailure());
        EXPECT_EQ(result4.getFinalFailure(), result3.getFinalFailure());
        EXPECT_EQ(result4.getAssertions().begin(), result3.getAssertions().begin());
        EXPECT_EQ(result4.getAssertions().end(), result3.getAssertions().end());

        // Create array of tests expected to fail. Terminate with nullptr.
        static const Assertion* fails[] = { fail1, fail2, nullptr };
        static Metadata<TestExpect> _1(*this, "expect", TestExpect::SomeFail);
        static Metadata<const Assertion**> _2(*this, "fails", static_cast<const Assertion**>(fails));
    }
}

TEST_SUITE(ResultSuite)

#include <cstring>

TEST(ResultSuite, ResultsTest)
{
    for (auto& suite : getSuites())
    {
        if (std::strcmp(suite.name, "_ResultSuite") != 0) continue;

        auto suiteInstance = suite.getSingletonSmartPtr();
        for (auto& test : suite.tests())
        {
            auto result = TestRunner(*suiteInstance, test).run();
            bool succeeded = testPassed(test, result);

            printTestResult(test, succeeded, result);
            EXPECT_ALL_OR_ASSERT(succeeded);
        }
        break;
    }
}
