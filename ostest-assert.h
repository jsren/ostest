/* ostest-assert.h - (c) James S Renwick 2016 */
#pragma once

#include "ostest-impl.h"

namespace _ostest_internal
{

#define _OSTEST_ASSERTION_DEF(name, successMsg, failMsg) \
    class _assert_ ## name : public ::ostest::Assertion \
    { \
    public: \
        inline _assert_ ## name(const char* file = __FILE__, int line = __LINE__) \
            : ::ostest::Assertion(file, line) { } \
        inline ~_assert_ ## name() override { } \
    \
    public: \
        const char* getMessage() const override { \
            return this->passed() ? successMsg : failMsg; \
        } \
    }; \

    _OSTEST_ASSERTION_DEF(ze,  "The value was zero.",        "Expected zero value.")
    _OSTEST_ASSERTION_DEF(nz,  "The value was non-zero.",    "Expected non-zero value.")
    _OSTEST_ASSERTION_DEF(eq,  "The values were equal.",     "Expected equal values.")
    _OSTEST_ASSERTION_DEF(neq, "The values were not equal.", "Expected non-equal values.")

    _OSTEST_ASSERTION_DEF(lt, "The first value was less than the second.",
        "The first value was not less than the second.")
    _OSTEST_ASSERTION_DEF(gt, "The first value was greater than the second.",
        "The first value was not greater than the second.")
    _OSTEST_ASSERTION_DEF(lte, "The first value was less than or equal to the second.",
        "The first value was not less than or equal to the second.")
    _OSTEST_ASSERTION_DEF(gte, "The first value was greater than or equal to the second.",
        "The first value was not greater than or equal to the second.")

#undef _OSTEST_ASSERTION_DEF


#define OSTEST_ASSERT_ZERO(expr)    _OSTEST_ASSERT_INT(__COUNT__, (expr)==0, ::_ostest_internal::_assert_ze)
#define OSTEST_ASSERT_NONZERO(expr) _OSTEST_ASSERT_INT(__COUNT__, (expr)!=0, ::_ostest_internal::_assert_nz)

#define OSTEST_ASSERT_EQ(expr1, expr2)   _OSTEST_ASSERT_INT(__COUNT__, (expr1)==(expr2), ::_ostest_internal::_assert_eq)
#define OSTEST_ASSERT_NEQ(expr1, expr2)  _OSTEST_ASSERT_INT(__COUNT__, (expr1)!=(expr2), ::_ostest_internal::_assert_neq)
#define OSTEST_ASSERT_LT(expr1, expr2)   _OSTEST_ASSERT_INT(__COUNT__, (expr1)<(expr2), ::_ostest_internal::_assert_lt)
#define OSTEST_ASSERT_GT(expr1, expr2)   _OSTEST_ASSERT_INT(__COUNT__, (expr1)>(expr2), ::_ostest_internal::_assert_gt)
#define OSTEST_ASSERT_LTEQ(expr1, expr2) _OSTEST_ASSERT_INT(__COUNT__, (expr1)<=(expr2), ::_ostest_internal::_assert_lte)
#define OSTEST_ASSERT_GTEQ(expr1, expr2) _OSTEST_ASSERT_INT(__COUNT__, (expr1)>=(expr2),::_ostest_internal:: _assert_gte)

#define OSTEST_EXPECT_ZERO(expr)    _OSTEST_EXPECT_INT(__COUNT__, (expr)==0, ::_ostest_internal::_assert_ze)
#define OSTEST_EXPECT_NONZERO(expr) _OSTEST_EXPECT_INT(__COUNT__, (expr)!=0, ::_ostest_internal::_assert_nz)

#define OSTEST_EXPECT_EQ(expr1, expr2)   _OSTEST_EXPECT_INT(__COUNT__, (expr1)==(expr2), ::_ostest_internal::_assert_eq)
#define OSTEST_EXPECT_NEQ(expr1, expr2)  _OSTEST_EXPECT_INT(__COUNT__, (expr1)!=(expr2), ::_ostest_internal::_assert_neq)
#define OSTEST_EXPECT_LT(expr1, expr2)   _OSTEST_EXPECT_INT(__COUNT__, (expr1)<(expr2), ::_ostest_internal::_assert_lt)
#define OSTEST_EXPECT_GT(expr1, expr2)   _OSTEST_EXPECT_INT(__COUNT__, (expr1)>(expr2), ::_ostest_internal::_assert_gt)
#define OSTEST_EXPECT_LTEQ(expr1, expr2) _OSTEST_EXPECT_INT(__COUNT__, (expr1)<=(expr2), ::_ostest_internal::_assert_lte)
#define OSTEST_EXPECT_GTEQ(expr1, expr2) _OSTEST_EXPECT_INT(__COUNT__, (expr1)>=(expr2), ::_ostest_internal::_assert_gte)

#ifndef OSTEST_USE_PREFIX
#define ASSERT_ZERO(expr) OSTEST_ASSERT_ZERO(expr)
#define ASSERT_NONZERO(expr) OSTEST_ASSERT_NONZERO(expr)

#define ASSERT_EQ(expr1, expr2) OSTEST_ASSERT_EQ(expr1, expr2)
#define ASSERT_NEQ(expr1, expr2) OSTEST_ASSERT_NEQ(expr1, expr2)
#define ASSERT_LT(expr1, expr2) OSTEST_ASSERT_LT(expr1, expr2)
#define ASSERT_GT(expr1, expr2) OSTEST_ASSERT_GT(expr1, expr2)
#define ASSERT_LTEQ(expr1, expr2) OSTEST_ASSERT_LTEQ(expr1, expr2)
#define ASSERT_GTEQ(expr1, expr2) OSTEST_ASSERT_GTEQ(expr1, expr2)

#define EXPECT_ZERO(expr) OSTEST_EXPECT_ZERO(expr)
#define EXPECT_NONZERO(expr) OSTEST_EXPECT_NONZERO(expr)

#define EXPECT_EQ(expr1, expr2) OSTEST_EXPECT_EQ(expr1, expr2)
#define EXPECT_NEQ(expr1, expr2) OSTEST_EXPECT_NEQ(expr1, expr2)
#define EXPECT_LT(expr1, expr2) OSTEST_EXPECT_LT(expr1, expr2)
#define EXPECT_GT(expr1, expr2) OSTEST_EXPECT_GT(expr1, expr2)
#define EXPECT_LTEQ(expr1, expr2) OSTEST_EXPECT_LTEQ(expr1, expr2)
#define EXPECT_GTEQ(expr1, expr2) OSTEST_EXPECT_GTEQ(expr1, expr2)
#endif

}
