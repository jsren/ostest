/* common.hpp - (c) 2018 James Renwick */
#include <ostest.hpp>
#include <stddef.h>
#include <string>

enum class TestExpect
{
    AllPass,
    AllFail
};

size_t countAssertions(const ostest::TestResult& result);

bool testShouldFail(const ostest::TestInfo& test);

bool allAssertionsFailed(const ostest::TestResult& result);

void printTestResult(const ostest::TestInfo& test, bool succeeded,
    const ostest::TestResult& result);


// Used to run tests in 'bare' profile
#if OSTEST_NO_ALLOC
#define EXPECT_ALL_OR_ASSERT(...) ASSERT(__VA_ARGS__)
#else
#define EXPECT_ALL_OR_ASSERT(...) EXPECT_ALL(__VA_ARGS__)
#endif
