/* ostest-assert.h - (c) James S Renwick 2016 */
#pragma once

#include "ostest-impl.h"


namespace _ostest_internal
{
#define _OSTEST_CONCAT(a, b) a ## b

    /* [internal] Creates a new ostest Unit Test Assertion. */
#define _OSTEST_ASSERT_INT(id, expr, cls) { static cls _OSTEST_CONCAT(_assertion, id)(#expr, __FILE__, __LINE__); \
                                     if (! _OSTEST_CONCAT(_assertion, id) .evaluate(*this, (expr))) return; }

/* [internal] Creates a new ostest Unit Test Expectation. */
#define _OSTEST_EXPECT_INT(id, expr, cls) { static cls _OSTEST_CONCAT(_assertion, id)(#expr, __FILE__, __LINE__); \
                                     _OSTEST_CONCAT(_assertion, id) .evaluate(*this, (expr)); }

/* [internal] Creates a new ostest Unit Test Expectation. */
#define _OSTEST_EXPECTBR_INT(id, expr, cls) { static cls _OSTEST_CONCAT(_assertion, id)(#expr, __FILE__, __LINE__); \
                                     if (!_OSTEST_CONCAT(_assertion, id) .evaluate(*this, (expr))) break; }

#if !OSTEST_NO_ALLOC
/* [internal] Creates a new ostest Unit Test Assertion. */
#define _OSTEST_ASSERT_ALL_INT(expr, cls) { cls* _assert = new cls(#expr, _ostest_internal::_heapalloc_tag{}, __FILE__, __LINE__, true); \
                                        if (!_assert->evaluate(*this, (expr))) return; }

/* [internal] Creates a new ostest Unit Test Expectation. */
#define _OSTEST_EXPECT_ALL_INT(expr, cls) { cls* _assert = new cls(#expr, _ostest_internal::_heapalloc_tag{}, __FILE__, __LINE__, true); \
                                        _assert->evaluate(*this, (expr)); }

/* [internal] Creates a new ostest Unit Test Expectation. */
#define _OSTEST_EXPECT_ALLBR_INT(expr, cls) { cls* _assert = new cls(#expr, _ostest_internal::_heapalloc_tag{}, __FILE__, __LINE__, true); \
                                        if (!_assert->evaluate(*this, (expr))) break; }

#else
// Define macros as being unavailable if OSTEST_NO_ALLOC defined
#define _OSTEST_ASSERT_ALL_INT(expr, cls) static_assert(false, "ASSERT_ALL unavailable when 'OSTEST_NO_ALLOC' defined.");
#define _OSTEST_EXPECT_ALL_INT(expr, cls) static_assert(false, "EXPECT_ALL unavailable when 'OSTEST_NO_ALLOC' defined.");
#define _OSTEST_EXPECT_ALLBR_INT(expr, cls) static_assert(false, "EXPECT_ALL_OR_BREAK unavailable when 'OSTEST_NO_ALLOC' defined.");
#endif


#define _OSTEST_ASSERTION_DEF(name, successMsg, failMsg) \
    class _assert_ ## name : public ::ostest::Assertion \
    { \
    public: \
        inline _assert_ ## name(const char* expr, const char* file = __FILE__, \
            int line = __LINE__, bool tmp = false) \
            : ::ostest::Assertion(expr, file, line, tmp) { } \
    \
    public: \
        const char* getMessage() const override final { \
            return this->passed() ? emptyMsg : failMsg; \
        } \
    };

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


/* Creates a new OSTest Unit Test Assertion. */
#define OSTEST_ASSERT(expr) _OSTEST_ASSERT_INT(__COUNTER__, expr, ::ostest::Assertion)
/* Creates a new OSTest Unit Test Expectation. */
#define OSTEST_EXPECT(expr) _OSTEST_EXPECT_INT(__COUNTER__, expr, ::ostest::Assertion)
/* Creates a new OSTest Unit Test Expectation. Breaks when condition false. */
#define OSTEST_EXPECT_OR_BREAK(expr) _OSTEST_EXPECTBR_INT(__COUNTER__, expr, ::ostest::Assertion)

/* Creates a new OSTest Unit Test Assertion. */
#define OSTEST_ASSERT_ALL(expr) _OSTEST_ASSERT_ALL_INT(expr, ::ostest::Assertion)
/* Creates a new OSTest Unit Test Expectation. */
#define OSTEST_EXPECT_ALL(expr) _OSTEST_EXPECT_ALL_INT(expr, ::ostest::Assertion)
/* Creates a new OSTest Unit Test Expectation. Breaks when condition false. */
#define OSTEST_EXPECT_ALL_OR_BREAK(expr) _OSTEST_EXPECT_ALLBR_INT(expr, ::ostest::Assertion)


#define OSTEST_ASSERT_ZERO(expr)    _OSTEST_ASSERT_INT(__COUNTER__, (expr) == 0, ::_ostest_internal::_assert_ze)
#define OSTEST_ASSERT_NONZERO(expr) _OSTEST_ASSERT_INT(__COUNTER__, (expr) != 0, ::_ostest_internal::_assert_nz)

#define OSTEST_ASSERT_EQ(expr1, expr2)   _OSTEST_ASSERT_INT(__COUNTER__, (expr1) ==(expr2), ::_ostest_internal::_assert_eq)
#define OSTEST_ASSERT_NEQ(expr1, expr2)  _OSTEST_ASSERT_INT(__COUNTER__, (expr1) != (expr2), ::_ostest_internal::_assert_neq)
#define OSTEST_ASSERT_LT(expr1, expr2)   _OSTEST_ASSERT_INT(__COUNTER__, (expr1) < (expr2), ::_ostest_internal::_assert_lt)
#define OSTEST_ASSERT_GT(expr1, expr2)   _OSTEST_ASSERT_INT(__COUNTER__, (expr1) > (expr2), ::_ostest_internal::_assert_gt)
#define OSTEST_ASSERT_LTEQ(expr1, expr2) _OSTEST_ASSERT_INT(__COUNTER__, (expr1) <= (expr2), ::_ostest_internal::_assert_lte)
#define OSTEST_ASSERT_GTEQ(expr1, expr2) _OSTEST_ASSERT_INT(__COUNTER__, (expr1) >= (expr2),::_ostest_internal:: _assert_gte)

#define OSTEST_EXPECT_ZERO(expr)    _OSTEST_EXPECT_INT(__COUNTER__, (expr) == 0, ::_ostest_internal::_assert_ze)
#define OSTEST_EXPECT_NONZERO(expr) _OSTEST_EXPECT_INT(__COUNTER__, (expr) != 0, ::_ostest_internal::_assert_nz)

#define OSTEST_EXPECT_EQ(expr1, expr2)   _OSTEST_EXPECT_INT(__COUNTER__, (expr1) == (expr2), ::_ostest_internal::_assert_eq)
#define OSTEST_EXPECT_NEQ(expr1, expr2)  _OSTEST_EXPECT_INT(__COUNTER__, (expr1) != (expr2), ::_ostest_internal::_assert_neq)
#define OSTEST_EXPECT_LT(expr1, expr2)   _OSTEST_EXPECT_INT(__COUNTER__, (expr1) < (expr2), ::_ostest_internal::_assert_lt)
#define OSTEST_EXPECT_GT(expr1, expr2)   _OSTEST_EXPECT_INT(__COUNTER__, (expr1) > expr2), ::_ostest_internal::_assert_gt)
#define OSTEST_EXPECT_LTEQ(expr1, expr2) _OSTEST_EXPECT_INT(__COUNTER__, (expr1) <= (expr2), ::_ostest_internal::_assert_lte)
#define OSTEST_EXPECT_GTEQ(expr1, expr2) _OSTEST_EXPECT_INT(__COUNTER__, (expr1) >= (expr2), ::_ostest_internal::_assert_gte)

#if !OSTEST_MUST_PREFIX

#define ASSERT(expr) OSTEST_ASSERT(expr)
#define EXPECT(expr) OSTEST_EXPECT(expr)
#define EXPECT_OR_BREAK(expr) OSTEST_EXPECT_OR_BREAK(expr)

#define ASSERT_ALL(expr) OSTEST_ASSERT_ALL(expr)
#define EXPECT_ALL(expr) OSTEST_EXPECT_ALL(expr)
#define EXPECT_ALL_OR_BREAK(expr) OSTEST_EXPECT_ALL_OR_BREAK(expr)

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
