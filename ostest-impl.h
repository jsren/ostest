/* ostest-impl.h - (c) 2017 James S Renwick */
#pragma once

namespace ostest
{
    class UnitTest;
    class TestInfo;
    class TestRunner;
    class TestResult;
    class TestEnumerator;
    class Assertion;
    class AssertionEnumerator;
    class ITestWrapper;

    // This must be provided by users
    /* Method called when a test has completed execution. */
    extern void handleTestComplete(const ostest::TestInfo& test,
        const ostest::TestResult& result) noexcept;
}

namespace _ostest_internal
{
    template<typename T>
    class _LinkedList
    {
    protected:
        T* firstItem = nullptr;  // The first item in the linked list.
        T* finalItem = nullptr;  // The last item in the linked list.
        unsigned long itemCount; // The number of items in the linked list.
    };

    template<typename T>
    class _StaticLinkedList
    {
    protected:
        T* nextItem = nullptr;          // The next item in the linked list.
        static T* firstItem;            // The first item in the linked list.
        static T* finalItem;            // The last item in the linked list.
        static unsigned long itemCount; // The number of items in the linked list.
    };

    template<typename T>
    T* _StaticLinkedList<T>::firstItem = nullptr;
    template<typename T>
    T* _StaticLinkedList<T>::finalItem = nullptr;
    template<typename T>
    unsigned long _StaticLinkedList<T>::itemCount = 0;

}

namespace ostest
{
    class Assertion
    {
        friend class AssertionEnumerator;
        friend class TestResult;

    private:
        bool result;
        char* message;

        Assertion* nextItem;
        Assertion* prevItem;

    protected:
        const bool temporary;

    public:
        const char* expression; // The assertion expression
        const char* file;       // The file in which the assertion is made
        const int line;         // The line at which the assertion is made

    public:
        /* [internal] Creates (but does not register) a new Assertion instance. */
        Assertion(const char* expression, const char* file = __FILE__, int line = __LINE__, bool temporary = false);
        virtual ~Assertion() { }

    protected:
        static constexpr const char* emptyMsg = "";

    public:
        /* Returns true if the assertion passed, false otherwise. */
        inline bool passed() const { return result; }

        /* Gets a description of the assertion outcome. */
        virtual const char* getMessage() const { 
            return result ? emptyMsg : "The assertion failed."; 
        }

        /* Performs a deep-copy of the current assertion. */
        virtual Assertion* clone() const
        {
            return new Assertion(*this);
        }

        /* Returns true if the assertion passed. False otherwise. */
        inline operator bool() const {
            return result;
        }

        /* [internal] Performs an assertion within the context of the given unit test. */
        bool assert(UnitTest& test, bool expression);
    };


    /* Object allowing assertion/expectation enumeration. */
    class AssertionEnumerator
    {
    private:
        bool initial;
        Assertion* item;

    public:
        /* Creates a new assertion/expectation enumerator. */
        inline AssertionEnumerator(Assertion* first) : initial(true), 
            item(first) { }

    public:
        /* Selects the next assertion in the collection. Returns false if none available.
        Call next() before current(). */
        bool next();

        /* Selects the previous assertion in the collection. Returns false if none available. */
        bool previous();

        /* Gets the next assertion in the collection. Call next() before accessing.
        Returns nullptr if none available or end of collection reached. */
        inline const Assertion* current() const {
            return item;
        }
    };

    /* Object holding the result data for a test. */
    class TestResult : private ::_ostest_internal::_LinkedList<Assertion>
    {
        friend class Assertion;

#if !OSTEST_NO_ALLOC
    private:
        unsigned int* refCount;

    public:
        TestResult();
        TestResult(const TestResult& copy);
        ~TestResult();
#endif

    public:
        /* Returns true if the test succeeded. False otherwise. */
        bool succeeded() const;

        /* Gets the first assertion/expectation to fail, or nullptr if none. */
        const Assertion* getFirstFailure() const;

        /* Gets the final assertion/expectation to fail, or nullptr if none. */
        const Assertion* getFinalFailure() const;

        /* Gets the assertions and expectations made by the test. */
        inline AssertionEnumerator getAssertions() const {
            return AssertionEnumerator(this->firstItem);
        }
        /* Returns true if the test succeeded. False otherwise. */
        inline operator bool() const { 
            return succeeded(); 
        }
    };


    class TestRunner
    {
    private:
        const TestInfo& info;

    public:
        TestRunner(const TestInfo& info) : info(info) { }
        virtual ~TestRunner() = default;

    public:
        virtual TestResult run();
    };
    
    class UnitTest
    {
        friend class Assertion;
        friend class TestRunner;

    private:
        TestResult result;
        const TestInfo* info;

    protected:
        /* Creates (but does not register) a new Unit Test. */
        inline UnitTest(const TestInfo& info) : result(), info(&info) { }
    public:
        virtual ~UnitTest() = default;

    protected:
        /* The Unit Test body. */
        virtual void testBody() = 0;

    public:
        /* Gets TestInfo for the current Unit Test. */
        inline const TestInfo& getInfo() const { return *info; }

        /* Gets the current TestResult for the current Unit Test. */
        inline const TestResult& getResult() const { return result; }
    };

    /* Object representing Test Suites. */
    class TestSuite
    {
        friend class TestRunner;

    public:
        virtual ~TestSuite() = 0; // Make class abstract

    protected:
        /* Executed before unit test run. */
        virtual void setUp() { }
        /* Executed after unit test run. */
        virtual void tearDown() { }
    };

    /* Object allowing test enumeration. */
    class TestEnumerator
    {
    private:
        bool initial;
        const char* suite;
        const TestInfo* item;

    public:
        /* Creates a new test enumerator. */
        inline TestEnumerator() : initial(true), suite(nullptr), 
            item(nullptr) { }
        /* Creates a new test enumerator filtering tests to those within the given test suite. */
        inline TestEnumerator(const char* suite) : initial(true), 
            suite(suite), item(nullptr) { }

    public:
        /* Selects the next test in the collection. Returns false if none available. 
        Call next() before current(). */
        bool next();

        /* Gets the next test in the collection. Call next() before accessing. 
        Returns nullptr if none available or end of collection reached. */
        inline const TestInfo* current() const {
            return item;
        }
    };

    /* Class containing unit test metadata. */
    class TestInfo : private _ostest_internal::_StaticLinkedList<TestInfo>
    {
        friend class TestEnumerator;
        friend class TestRunner;

    private:
        ::ostest::ITestWrapper& wrapper;

    public:
        const int line;              // The line of the test definition.
        const char* const file;      // The file containing the test definition.
        const char* const suiteName; // The name of the test suite containing the test.
        const char* const testName;  // The name of the specific test.

    private:
        /* Creates and registers a new TestInfo instance. */
        TestInfo(const char* suiteName, const char* testName, ITestWrapper& wrapper,
            const char* file = __FILE__, int line = __LINE__);

    public:
        // Default move constructor
        TestInfo(TestInfo&& move) noexcept = default;
        // Default copy constructor
        TestInfo(const TestInfo& copy) noexcept = default;

    public:
        /* Creates and registers a new unit test with the given details.
           Returns the new test's test info.
        */
        static const TestInfo registerNew(const char* suiteName, const char* testName,
            ITestWrapper& wrapper, const char* file = __FILE__, int line = __LINE__);
    };

    /* Internal interface managing test lifetimes. */
    class ITestWrapper
    {
        friend class TestRunner;

    protected:
        virtual ::ostest::UnitTest& getTest() = 0;
        virtual ::ostest::TestSuite& getSuite() = 0;
        virtual void newInstance() = 0;
        virtual void deleteInstance() = 0;
    };
}

namespace _ostest_internal
{
    template<typename T>
    class _TestWrapper : public ::ostest::ITestWrapper
    {
    private:
        bool alloc;

        union Data { 
            alignas(alignof(T)) char _data[sizeof(T)]; T _obj;
            inline Data() noexcept : _data() { } inline ~Data() { };
        } data;

    public:
        inline _TestWrapper() : alloc(false) /*leave data uninitialised*/ { }
        inline ~_TestWrapper() { deleteInstance(); }

    protected:
        ::ostest::UnitTest& getTest() override
        {
            if (!alloc) newInstance();
            return data._obj;
        }
        ::ostest::TestSuite& getSuite() override
        { 
            if (!alloc) newInstance();
            return data._obj;
        }

        void newInstance() override
        {
            if (alloc) deleteInstance();

            new (data._data) T();
            alloc = true;
        }

        void deleteInstance() override
        {
            if (alloc) return data._obj.T::~T();
            alloc = false;
        }
    };
}


#pragma region Internal

// Gets the internal class name for the new unit test
#define _OSTEST_CLS_NAME(s,t) _test ## s ## t

// Gets the internal namespace for the new unit test
#define _OSTEST_NS _tests


/* [internal] Creates a new OSTest Unit Test. */
#define _OSTEST_INTERNAL(suiteClass, suiteName, testName) \
    namespace _OSTEST_NS { \
        class _OSTEST_CLS_NAME(suiteName, testName) : \
            public suiteClass, public ::ostest::UnitTest\
        { \
            friend class ::ostest::TestRunner; \
        private: \
            static const ostest::TestInfo info; \
            static ::_ostest_internal::_TestWrapper<_OSTEST_NS::_OSTEST_CLS_NAME(suiteName, testName)> _wrapper; \
        public: \
            inline _OSTEST_CLS_NAME(suiteName, testName)() noexcept : ::ostest::UnitTest(info) { } \
            inline void* operator new(size_t, void* where) noexcept { return where; } \
        protected: \
            virtual void testBody() override; \
        }; \
    } \
    ::_ostest_internal::_TestWrapper<_OSTEST_NS::_OSTEST_CLS_NAME(suiteName, testName)> _OSTEST_NS::_OSTEST_CLS_NAME(suiteName, testName)::_wrapper{}; \
    \
    const ::ostest::TestInfo _OSTEST_NS::_OSTEST_CLS_NAME(suiteName, testName)::info = ::ostest::TestInfo::registerNew( \
        #suiteName, #testName, *static_cast<::ostest::ITestWrapper*>(&_OSTEST_NS::_OSTEST_CLS_NAME(suiteName, testName)::_wrapper), \
        __FILE__, __LINE__); \
    \
    void _OSTEST_NS::_OSTEST_CLS_NAME(suiteName, testName)::testBody()


#pragma endregion


/* Creates a new OSTest Unit Test. */
#define OSTEST_TEST(suiteName, testName) _OSTEST_INTERNAL(suiteName, suiteName, testName)

/* Creates a new OSTest Unit Test. */
#define OSTEST_TEST_EX(suiteNamespace, suiteName, testName) _OSTEST_INTERNAL(suiteNamespace::suiteName, suiteName, testName)

/* Creates a new OSTest Test Suite with empty setUp/tearDown. */
#define OSTEST_TEST_SUITE(suiteName) class suiteName : public ::ostest::TestSuite { };


#if !OSTEST_MUST_PREFIX
#define TEST(suiteName, testName) OSTEST_TEST(suiteName, testName)
#define TEST_EX(suiteNamespace, suiteName, testName) OSTEST_TEST_EX(suiteNamespace, suiteName, testName)

#define TEST_SUITE(suiteName) OSTEST_TEST_SUITE(suiteName)
#endif
