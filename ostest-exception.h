/* ostest-exception.h - (c) James S Renwick 2016 */
#pragma once

#if OSTEST_STD_EXCEPTIONS

#if OSTEST_NO_ALLOC
#error OSTEST: Cannot enable exception support when OSTEST_NO_ALLOC defined
#endif

#include "ostest-impl.h"
#include <exception>
#include <stdexcept>

namespace ostest
{
    /* Class representing an assertion over an exception not having been thrown. */
    class NoExceptionAssertion : public Assertion
    {
    private:
        const char* exceptionMsg;
    public:
        explicit NoExceptionAssertion(const std::exception& exception, 
            const TestInfo& test, bool temp=true);
        virtual ~NoExceptionAssertion();

        NoExceptionAssertion(NoExceptionAssertion&& other);
        NoExceptionAssertion(const NoExceptionAssertion& other) = delete;
        NoExceptionAssertion& operator=(const NoExceptionAssertion& other) = delete;

    public:
        /* Gets the exception message. */
        virtual const char* getMessage() const override;
    };
}

#endif
