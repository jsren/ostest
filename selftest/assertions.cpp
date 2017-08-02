/* assertions.cpp - (c) 2017 James S Renwick */
#include "common.hpp"

using namespace ostest;

namespace selftest
{
    TEST_SUITE(_AssertionSuite)

    TEST_EX(::selftest, _AssertionSuite, _TestAssertPass) {
        ASSERT(1 == 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertFail) {
        ASSERT(1 != 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertAllPass) {
        for (int i = 0; i < 100; i++) {
            ASSERT_ALL(i < 100);
        }
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertAllFail) {
        for (int i = 0; i < 10; i++) {
            ASSERT_ALL(i < 5);
        }
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertZeroPass) {
        ASSERT_ZERO(0);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertZeroFail) {
        ASSERT_ZERO(1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertNonZero) {
        ASSERT_NONZERO(1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertNonZeroFail) {
        ASSERT_NONZERO(0);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertEqPass) {
        ASSERT_EQ(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertEqFail) {
        ASSERT_EQ(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertNeqPass) {
        ASSERT_NEQ(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertNeqFail) {
        ASSERT_NEQ(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertLtPass) {
        ASSERT_LT(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertLt1Fail) {
        ASSERT_LT(2, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertLt2Fail) {
        ASSERT_LT(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertGtPass) {
        ASSERT_GT(2, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertGt1Fail) {
        ASSERT_GT(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertGt2Fail) {
        ASSERT_GT(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertLtEq) {
        ASSERT_LTEQ(1, 2);
        ASSERT_LTEQ(2, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertLtEqFail) {
        ASSERT_LTEQ(3, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertGtEq) {
        ASSERT_GTEQ(2, 2);
        ASSERT_GTEQ(3, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestAssertGtEqFail) {
        ASSERT_GTEQ(1, 2);
    }

    TEST_EX(::selftest, _AssertionSuite, _TestExpectPass) {
        EXPECT(1 == 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectFail) {
        EXPECT(1 != 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectAllPass) {
        for (int i = 0; i < 100; i++) {
            EXPECT_ALL(i < 100);
        }
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectAllFail) {
        for (int i = 0; i < 10; i++) {
            EXPECT_ALL(i < 5);
        }
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectZeroPass) {
        EXPECT_ZERO(0);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectZeroFail) {
        EXPECT_ZERO(1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectNonZero) {
        EXPECT_NONZERO(1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectNonZeroFail) {
        EXPECT_NONZERO(0);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectEqPass) {
        EXPECT_EQ(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectEqFail) {
        EXPECT_EQ(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectNeqPass) {
        EXPECT_NEQ(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectNeqFail) {
        EXPECT_NEQ(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectLtPass) {
        EXPECT_LT(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectLt1Fail) {
        EXPECT_LT(2, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectLt2Fail) {
        EXPECT_LT(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectGtPass) {
        EXPECT_GT(2, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectGt1Fail) {
        EXPECT_GT(1, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectGt2Fail) {
        EXPECT_GT(1, 1);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectLtEq) {
        EXPECT_LTEQ(1, 2);
        EXPECT_LTEQ(2, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectLtEqFail) {
        EXPECT_LTEQ(3, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectGtEq) {
        EXPECT_GTEQ(2, 2);
        EXPECT_GTEQ(3, 2);
    }
    TEST_EX(::selftest, _AssertionSuite, _TestExpectGtEqFail) {
        EXPECT_GTEQ(1, 2);
    }
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
            printTestResult(test, !result.succeeded(), result);
            EXPECT_ALL(!result.succeeded());
        }
        else {
            printTestResult(test, result.succeeded(), result);
            EXPECT_ALL(result.succeeded());
        }
    }
}
