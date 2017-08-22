/* assertion-test.cpp - (c) 2017 James S Renwick */
#include "common.hpp"

using namespace ostest;

namespace selftest
{
#define TEST_EXPECT_ALL_FAIL \
    static Metadata<TestExpect> _(*this, "expect", TestExpect::AllFail)


    TEST_SUITE(_AssertionSuite)

    TEST_EX(::selftest, _AssertionSuite, _TestAssertPass) {
        ASSERT(1 == 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertFail1)
    {
        TEST_EXPECT_ALL_FAIL;
        ASSERT(1 != 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertFail2) {
        TEST_EXPECT_ALL_FAIL;
        ASSERT(1 != 1);
        EXPECT(true);
    }
#if !OSTEST_NO_ALLOC
    TEST_EX(::selftest, _AssertionSuite, _TestAssertionCountPass1)
    {
        EXPECT_ONCE(true);
        EXPECT_EQ_ONCE(countAssertions(this->getResult()), 1);
        EXPECT_EQ_ONCE(countAssertions(this->getResult()), 2);
        EXPECT_EQ_ALL(countAssertions(this->getResult()), 3);
        EXPECT_EQ_ALL(countAssertions(this->getResult()), 4);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertionCountPass2)
    {
        EXPECT_ONCE(true);
        EXPECT_EQ_ONCE(countAssertions(this->getResult()), 1);

        for (int i = 0; i < 10; i++) {
            EXPECT_ALL(true);
        }
        EXPECT_EQ_ONCE(countAssertions(this->getResult()), 12);
        EXPECT_EQ_ALL(countAssertions(this->getResult()), 13);
        EXPECT_EQ_ALL(countAssertions(this->getResult()), 14);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertionCountPass3)
    {
        ASSERT_ONCE(true);
        ASSERT_EQ_ONCE(countAssertions(this->getResult()), 1);

        for (int i = 0; i < 10; i++) {
            ASSERT_ALL(true);
        }
        ASSERT_EQ_ONCE(countAssertions(this->getResult()), 12);
        ASSERT_EQ_ALL(countAssertions(this->getResult()), 13);
        ASSERT_EQ_ALL(countAssertions(this->getResult()), 14);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertionCountPass4)
    {
        ASSERT_ONCE(true);
        EXPECT_EQ_ONCE(countAssertions(this->getResult()), 1);

        for (int i = 0; i < 10; i++) {
            ASSERT_ALL(true);
            EXPECT_ALL(true);
        }
        ASSERT_EQ_ONCE(countAssertions(this->getResult()), 22);
        EXPECT_EQ_ALL(countAssertions(this->getResult()), 23);
        ASSERT_EQ_ALL(countAssertions(this->getResult()), 24);
    }
#endif
    TEST_EX(::selftest, _AssertionSuite, _TestAssertionCountPass5)
    {
        EXPECT_ZERO(countAssertions(this->getResult()));
        EXPECT_ONCE(true);
        EXPECT_EQ(countAssertions(this->getResult()), 2);
        EXPECT_EQ(countAssertions(this->getResult()), 3);
    }
#if !OSTEST_NO_ALLOC
    TEST_EX(::selftest, _AssertionSuite, _TestAssertAllPass) {
        for (int i = 0; i < 100; i++) {
            ASSERT_ALL(i < 100);
        }
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertAllFail) {
        TEST_EXPECT_ALL_FAIL;
        for (int i = 0; i < 10; i++) {
            ASSERT_ALL(i < 0);
        }
    }
#endif
    TEST_EX(::selftest, _AssertionSuite, _TestAssertZeroPass) {
        ASSERT_ZERO(0);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertZeroFail) {
        TEST_EXPECT_ALL_FAIL;
        ASSERT_ZERO(1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertNonZero) {
        ASSERT_NONZERO(1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertNonZeroFail) {
        TEST_EXPECT_ALL_FAIL;
        ASSERT_NONZERO(0);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertEqPass) {
        ASSERT_EQ(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertEqFail) {
        TEST_EXPECT_ALL_FAIL;
        ASSERT_EQ(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertNeqPass) {
        ASSERT_NEQ(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertNeqFail) {
        TEST_EXPECT_ALL_FAIL;
        ASSERT_NEQ(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertLtPass) {
        ASSERT_LT(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertLt1Fail) {
        TEST_EXPECT_ALL_FAIL;
        ASSERT_LT(2, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertLt2Fail) {
        TEST_EXPECT_ALL_FAIL;
        ASSERT_LT(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertGtPass) {
        ASSERT_GT(2, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertGtFail1) {
        TEST_EXPECT_ALL_FAIL;
        ASSERT_GT(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertGtFail2) {
        TEST_EXPECT_ALL_FAIL;
        ASSERT_GT(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertLtEq) {
        ASSERT_LTEQ(1, 2);
        ASSERT_LTEQ(2, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertLtEqFail) {
        TEST_EXPECT_ALL_FAIL;
        ASSERT_LTEQ(3, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertGtEq) {
        ASSERT_GTEQ(2, 2);
        ASSERT_GTEQ(3, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertGtEqFail) {
        TEST_EXPECT_ALL_FAIL;
        ASSERT_GTEQ(1, 2);
    }

    TEST_EX(::selftest, _AssertionSuite, _TestExpectPass) {
        EXPECT(1 == 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectFail) {
        TEST_EXPECT_ALL_FAIL;
        EXPECT(1 != 1);
    }
#if !OSTEST_NO_ALLOC
    TEST_EX(::selftest, _AssertionSuite, _TestExpectAllPass) {
        for (int i = 0; i < 100; i++) {
            EXPECT_ALL(i < 100);
        }
        EXPECT_EQ_ONCE(countAssertions(this->getResult()), 100);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectAllFail) {
        TEST_EXPECT_ALL_FAIL;
        for (int i = 0; i < 10; i++) {
            EXPECT_ALL(i < 0);
        }
        EXPECT_NEQ_ONCE(countAssertions(this->getResult()), 10);
    }
#endif
    TEST_EX(::selftest, _AssertionSuite, _TestExpectZeroPass) {
        EXPECT_ZERO(0);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectZeroFail) {
        TEST_EXPECT_ALL_FAIL;
        EXPECT_ZERO(1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectNonZero) {
        EXPECT_NONZERO(1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectNonZeroFail) {
        TEST_EXPECT_ALL_FAIL;
        EXPECT_NONZERO(0);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectEqPass) {
        EXPECT_EQ(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectEqFail) {
        TEST_EXPECT_ALL_FAIL;
        EXPECT_EQ(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectNeqPass) {
        EXPECT_NEQ(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectNeqFail) {
        TEST_EXPECT_ALL_FAIL;
        EXPECT_NEQ(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectLtPass) {
        EXPECT_LT(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectLt1Fail) {
        TEST_EXPECT_ALL_FAIL;
        EXPECT_LT(2, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectLt2Fail) {
        TEST_EXPECT_ALL_FAIL;
        EXPECT_LT(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectGtPass) {
        EXPECT_GT(2, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectGt1Fail) {
        TEST_EXPECT_ALL_FAIL;
        EXPECT_GT(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectGt2Fail) {
        TEST_EXPECT_ALL_FAIL;
        EXPECT_GT(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectLtEq) {
        EXPECT_LTEQ(1, 2);
        EXPECT_LTEQ(2, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectLtEqFail) {
        TEST_EXPECT_ALL_FAIL;
        EXPECT_LTEQ(3, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectGtEq) {
        EXPECT_GTEQ(2, 2);
        EXPECT_GTEQ(3, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectGtEqFail) {
        TEST_EXPECT_ALL_FAIL;
        EXPECT_GTEQ(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectOnceOrBreakPass)
    {
        int iteration = 0;
        for (; iteration < 10; iteration++) {
            OSTEST_EXPECT_ONCE_OR_BREAK(iteration < 10);
        }
        EXPECT_EQ(iteration, 10);
        EXPECT_EQ(countAssertions(this->getResult()), 2);
    }
#if !OSTEST_NO_ALLOC
    TEST_EX(::selftest, _AssertionSuite, _TestExpectAllOrBreakPass)
    {
        int iteration = 0;
        for (; iteration < 10; iteration++) {
            OSTEST_EXPECT_ALL_OR_BREAK(iteration < 10);
        }
        EXPECT_EQ(iteration, 10);
        EXPECT_EQ(countAssertions(this->getResult()), 11);
    }
#endif
    TEST_EX(::selftest, _AssertionSuite, _TestExpectOnceOrBreakFail)
    {
        TEST_EXPECT_ALL_FAIL;
        int iteration = 0;
        for (; iteration < 10; iteration++) {
            OSTEST_EXPECT_ONCE_OR_BREAK(iteration < 5);
        }
        EXPECT_NEQ(iteration, 5);
        EXPECT_NEQ(countAssertions(this->getResult()), 2);
    }
#if !OSTEST_NO_ALLOC
    TEST_EX(::selftest, _AssertionSuite, _TestExpectAllOrBreakFail)
    {
        TEST_EXPECT_ALL_FAIL;
        int iteration = 0;
        for (; iteration < 10; iteration++) {
            OSTEST_EXPECT_ALL_OR_BREAK(iteration < 0);
        }
        EXPECT_NEQ(iteration, 0);
        EXPECT_NEQ(countAssertions(this->getResult()), 2);
    }
#endif
}


TEST_SUITE(AssertionSuite)

TEST(AssertionSuite, AssertionsTest)
{
    auto tests = getUnitTests("_AssertionSuite");
    while (tests.next())
    {
        auto& test = tests.current();
        auto result = TestRunner(test).run();

        if (testShouldFail(test))
        {
            bool failed = !result.succeeded() && allAssertionsFailed(result);
            printTestResult(test, failed, result);
            EXPECT_ALL_OR_ASSERT(failed);
        }
        else {
            printTestResult(test, result.succeeded(), result);
            EXPECT_ALL_OR_ASSERT(result.succeeded());
        }
    }
}
