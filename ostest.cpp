/* ostest.cpp - (c) 2016-2018 James S Renwick */
#include "ostest-impl.hpp"

// Headers required for standard library exceptions
#if OSTEST_STD_EXCEPTIONS
#include <stdexcept>
#include <string>
#if OSTEST_NO_ALLOC
#error Exception support requires allocation: cannot compile with both OSTEST_STD_EXCEPTIONS and OSTEST_NO_ALLOC.
#endif
#endif


namespace _ostest_internal
{
    _MetadataItem::_MetadataItem(ostest::UnitTest& test,
        const char* name, void* item) : name(name), test(test), item(item)
    {
        test.addMetadata(*this);
    }

    _MetadataItem::~_MetadataItem() {
        test.removeMetadata(*this);
    }
}


namespace ostest
{
    // Set feature flags
#if OSTEST_STD_EXCEPTIONS
    extern const bool ostest_std_exceptions = true;
#else
    extern const bool ostest_std_exceptions = false;
#endif
#if OSTEST_NO_ALLOC
    extern const bool ostest_no_alloc = true;
#else
    extern const bool ostest_no_alloc = false;
#endif


#if OSTEST_STD_EXCEPTIONS

    /* Class representing an assertion that an exception has not been thrown. */
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
#endif


    // Abstract base class
    TestSuite::~TestSuite() { }


    // Tests for string equality
    static bool streq(const char* s1, const char* s2)
    {
        for (unsigned long int i = 0; ; i++)
        {
            if (s1[i] != s2[i]) return false;
            else if (s1[i] == s2[i] && s1[i] == '\0') return true;
        }
        return false;
    }

    // Typedef for convenience
    typedef _ostest_internal::_MetadataItem MetadataItem;


    void UnitTest::addMetadata(MetadataItem& item, bool user)
    {
        MetadataItem* root = user ? firstUserMetadataItem :
            firstInternalMetadataItem;

        if (root == nullptr) {
            (user ? firstUserMetadataItem : firstInternalMetadataItem) = &item;
        }
        else
        {
            MetadataItem* last = root;
            while (last->nextItem != nullptr) last = last->nextItem;
            last->nextItem = &item;
        }
    }

    void UnitTest::removeMetadata(MetadataItem& item, bool user)
    {
        MetadataItem* prev = user ? firstUserMetadataItem :
            firstInternalMetadataItem;

        if (prev == &item) {
            (user ? firstUserMetadataItem : firstInternalMetadataItem)
                = item.nextItem;
            return;
        }
        while (prev != nullptr && prev->nextItem != &item) {
            prev = prev->nextItem;
        }
        if (prev == nullptr) return;
        else prev->nextItem = item.nextItem;
    }

    void* UnitTest::getMetadataRaw(const char* name, bool user) const
    {
        MetadataItem* item = user ? firstUserMetadataItem :
            firstInternalMetadataItem;

        while (item != nullptr)
        {
            if (streq(item->name, name)) return item->item;
            else item = item->nextItem;
        }
        return nullptr;
    }

    // Creates a new assertion
    Assertion::Assertion(const char* expr, const char* file, int line, bool tmp)
        : temporary(tmp), expression(expr), file(file), line(line) { }

// Only define this overload if allocation enabled - prevents use of features which
// require allocation when allocation not enabled
#ifndef OSTEST_NO_ALLOC
    Assertion::Assertion(const char* expr, _ostest_internal::_heapalloc_tag,
        const char* file, int line, bool tmp) : Assertion(expr, file, line, tmp) { }
#endif

    bool Assertion::evaluate(UnitTest& test, bool result)
    {
        // Reset state in case values already set by previous invocation.
        // (Linked list state should have been reset by TestResult destructor.)
        this->result = result;

        // This function always inserts the assertion at the end of the list.
        // Thus if already present in list, update existing list pointers.
        if (this->nextItem != nullptr) this->nextItem->prevItem = this->prevItem;
        if (this->prevItem != nullptr) this->prevItem->nextItem = this->nextItem;

        // Update first item
        if (this->prevItem == nullptr && this->nextItem != nullptr) {
            test.result.firstItem = this->nextItem;
        }

        // Add to end of list
        this->prevItem = nullptr;
        this->nextItem = nullptr;

        // Update list first and final pointers
        if (test.result.firstItem == nullptr) test.result.firstItem = this;
        if (test.result.finalItem != nullptr && test.result.finalItem != this)
        {
            this->prevItem = test.result.finalItem;
            test.result.finalItem->nextItem = this;
        }
        test.result.finalItem = this;

        return result;
    }

#if OSTEST_NO_ALLOC
    TestResult::TestResult() = default;
    TestResult::TestResult(const TestResult&) = default;
    TestResult::TestResult(TestResult&&) = default;
    TestResult& TestResult::operator =(const TestResult&) = default;
    TestResult& TestResult::operator =(TestResult&&) = default;
    TestResult::~TestResult() = default;
#else
    TestResult::TestResult() : refCount(new unsigned int(1))
    {

    }

    TestResult::TestResult(TestResult&& other)
    {
        this->firstItem = other.firstItem;
        this->finalItem = other.finalItem;
        this->itemCount = other.itemCount;
        this->refCount  = other.refCount;

        other.firstItem = nullptr;
        other.finalItem = nullptr;
        other.itemCount = 0;
        other.refCount = nullptr;
    }

    TestResult::TestResult(const TestResult& copy)
    {
        this->firstItem = copy.firstItem;
        this->finalItem = copy.finalItem;
        this->itemCount = copy.itemCount;
        this->refCount  = copy.refCount;

        if (refCount != nullptr) { (*refCount)++; }
    }

    TestResult& TestResult::operator =(const TestResult& other)
    {
        if (&other == this) return *this; // Do nothing if same object

        this->destroy(); // Destroy existing result

        this->firstItem = other.firstItem;
        this->finalItem = other.finalItem;
        this->itemCount = other.itemCount;
        this->refCount  = other.refCount;

        if (refCount != nullptr) { (*refCount)++; }
        return *this;
    }

    TestResult& TestResult::operator =(TestResult&& other)
    {
        if (&other == this) return *this; // Do nothing if same object

        auto firstItem = this->firstItem;
        auto finalItem = this->finalItem;
        auto itemCount = this->itemCount;
        auto refCount = this->refCount;

        this->firstItem = other.firstItem;
        this->finalItem = other.finalItem;
        this->itemCount = other.itemCount;
        this->refCount  = other.refCount;

        other.firstItem = firstItem;
        other.finalItem = finalItem;
        other.itemCount = itemCount;
        other.refCount = refCount;

        return *this;
    }

    void TestResult::destroy()
    {
        // This is the case upon move
        if (this->refCount != nullptr)
        {
            this->refCount[0]--;

            if (*this->refCount == 0)
            {
                Assertion *next = this->firstItem;
                while (next != nullptr)
                {
                    Assertion* tmp = next->nextItem;

                    // Delete or reset
                    if (next->temporary) delete next;
                    else next->prevItem = next->nextItem = nullptr;

                    next = tmp; // Return next assertion
                }
            }
        }
    }

    TestResult::~TestResult()
    {
        this->destroy();
    }
#endif

    bool TestResult::succeeded() const
    {
        for (auto& assertion : AssertionIterator(this->firstItem)) {
            if (!assertion.passed()) return false;
        }
        return true;
    }

    const Assertion* TestResult::getFirstFailure() const
    {
        for (auto& assertion : AssertionIterator(this->firstItem)) {
            if (!assertion.passed()) return &assertion;
        }
        return nullptr;
    }

    const Assertion* TestResult::getFinalFailure() const
    {
        const Assertion* final = nullptr;

        for (auto& assertion : AssertionIterator(this->firstItem)) {
            if (!assertion.passed()) final = &assertion;
        }
        return final;
    }

    SuiteUniquePtr::SuiteUniquePtr(SuiteInfo& info)
        : info(info), instance(info.construct()) { }

    SuiteUniquePtr::~SuiteUniquePtr() {
        info.destruct();
    }


    SuiteInfo::SuiteInfo(void* ptr, ctor constructor, dtor destructor, const char* name)
        : ptr(ptr), constructor(constructor), destructor(destructor), name(name)
    {
        if (firstItem == nullptr) firstItem = this;
        else if (finalItem != nullptr) finalItem->nextItem = this;
        finalItem = this;
        itemCount++;
    }

    /* Creates and registers a new unit test with the given details.
       Returns the new test's test info.
    */
    TestInfo TestInfo::registerNew(SuiteInfo& suite, const char* name,
        UnitTestWrapper& wrapper, const char* file, int line)
    {
        return TestInfo(suite, name, wrapper, file, line);
    }

    TestInfo::TestInfo(SuiteInfo& suite, const char* name,
        UnitTestWrapper& wrapper, const char* file, int line)
        : wrapper(wrapper), line(line), suite(suite), name(name), file(file)
    {
        // Register test with suite
        suite._tests.addItem(this);
    }

    TestResult TestRunner::run()
    {
        // Get the test instance
        UnitTest& test = info.wrapper.newInstance(suite);

        // Perform testing
        suite.setUp();

#if OSTEST_STD_EXCEPTIONS
        try {
            test.testBody();
        }
        catch (const std::exception& e) {
            (new NoExceptionAssertion(e, test.getInfo()))->evaluate(test, false);
        }
        // Please do not use 'new' when throwing exceptions!
        // This will NOT free the exception. This is to avoid aborting later.
        catch (const std::exception* e) {
            (new NoExceptionAssertion(*e, test.getInfo()))->evaluate(test, false);
        }
        catch (const std::string& str) {
            (new NoExceptionAssertion(std::runtime_error(str.c_str()),
                test.getInfo()))->evaluate(test, false);
        }
        catch (const char* msg) {
            (new NoExceptionAssertion(std::runtime_error(msg), test.getInfo()))->evaluate(test, false);
        }
        catch (...) {
            (new NoExceptionAssertion(std::exception(), test.getInfo()))->evaluate(test, false);
        }
#else
        test.testBody();
#endif
        suite.tearDown();

        // Clean up
        TestResult result = test.result;
        info.wrapper.deleteInstance();

        // Notify test complete
        ::ostest::handleTestComplete(info, result);
        return result;
    }

    SuiteIterator getSuites() noexcept
    {
        return SuiteIterator{SuiteInfo::firstItem};
    }


#if OSTEST_STD_EXCEPTIONS

    static const char noexceptionMsg[] = "An unhandled exception occurred: ";

    NoExceptionAssertion::NoExceptionAssertion(const std::exception& exception,
        const TestInfo& test, bool temp) : Assertion("<unhandled exception>",
            test.file, test.line, temp)
    {
        auto what = exception.what();

        // Get exception message string length (up to 816)
        unsigned long length = 0;
        for (; length <= 816; length++) {
            if (what[length] == '\0') break;
        }

        unsigned long msgIndex = 0;
        auto msg = new char[sizeof(noexceptionMsg) + length];

        // Copy message prefix
        for (; msgIndex < sizeof(noexceptionMsg) - 1; msgIndex++) {
            msg[msgIndex] = noexceptionMsg[msgIndex];
        }
        // Copy exception message
        for (unsigned long i = 0; i < length + 1; i++, msgIndex++) {
            msg[msgIndex] = what[i];
        }
        this->exceptionMsg = msg;
    }

    NoExceptionAssertion::~NoExceptionAssertion() {
        if (this->exceptionMsg != nullptr) delete[] this->exceptionMsg;
    }

    NoExceptionAssertion::NoExceptionAssertion(NoExceptionAssertion&& other)
        : Assertion(other.expression, other.file, other.line, other.temporary)
    {
        this->exceptionMsg = other.exceptionMsg;
        other.exceptionMsg = nullptr;
    }

    const char* NoExceptionAssertion::getMessage() const
    {
        return this->exceptionMsg;
    }

#endif
}
