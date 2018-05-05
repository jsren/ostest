/* ostest-impl.hpp - (c) 2018 James Renwick */
#pragma once

namespace ostest
{
    class UnitTest;
    class TestInfo;
    class SuiteInfo;
    class TestSuite;
    class TestRunner;
    class TestResult;
    class TestEnumerator;
    class Assertion;
    class UnitTestWrapper;

    template<typename T>
    class Iterable;
    using AssertionIterator = Iterable<Assertion>;
    using SuiteIterator = Iterable<SuiteInfo>;
    using TestIterator = Iterable<const TestInfo>;

    // This must be provided by users
    /* Method called when a test has completed execution. */
    extern void handleTestComplete(const ostest::TestInfo& test,
        const ostest::TestResult& result);
}

namespace _ostest_internal
{
    using size_t = decltype(sizeof(int));

    // Necessary parts from type_traits
    template<bool B>
    struct bool_constant {
        static constexpr const bool value = B;
    };
    using true_type = bool_constant<true>;
    using false_type = bool_constant<false>;

    template<typename T>
    struct is_pointer_type : false_type { };
    template<typename T>
    struct is_pointer_type<T*> : true_type { };
    template<typename T>
    struct is_pointer_type<T*&> : true_type { };
    template<typename T>
    struct is_pointer_type<T*&&> : true_type { };
    template<typename T>
    struct is_pointer_type<T* const> : true_type { };
    template<typename T>
    struct is_pointer_type<T* volatile> : true_type { };
    template<typename T>
    struct is_pointer_type<T* const volatile> : true_type { };
    template<typename T>
    struct is_pointer_type<T[]> : true_type { };

    template<typename T>
    struct is_test_type {
    private:
        template<typename Y, class = decltype(static_cast<ostest::UnitTest*>(Y{nullptr}))>
        static true_type test(int);
        template<typename>
        static false_type test(...);

    public:
        static constexpr const bool value = decltype(test<T*>(0))::value;
    };

    template<typename T>
    struct remove_reference { using type = T; };
    template<typename T>
    struct remove_reference<T&> { using type = T; };
    template<typename T>
    struct remove_reference<T&&> { using type = T; };

    template<typename T>
    constexpr inline T&& forward(typename remove_reference<T>::type& ref) noexcept
    {
        return static_cast<T&&>(ref);
    }
    template<typename T>
    constexpr inline T&& forward(typename remove_reference<T>::type&& ref) noexcept
    {
        return static_cast<T&&>(ref);
    }

    template<typename T>
    class _LinkedListIterator;

    template<typename T>
    class _LinkedList
    {
        friend _LinkedListIterator<T>;

    public:
        T* firstItem = nullptr;  // The first item in the linked list.
        T* finalItem = nullptr;  // The last item in the linked list.
        unsigned long itemCount; // The number of items in the linked list.

        void addItem(T* item)
        {
            if (firstItem == nullptr) firstItem = item;
            if (finalItem != nullptr) finalItem->nextItem = item;
            finalItem = item;
            itemCount++;
        }
    };

    template<typename T>
    class _StaticLinkedList
    {
        friend _LinkedListIterator<T>;

    public:
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

    // Type used to select an overload when the object is heap-allocated
    struct _heapalloc_tag { };

    // Type representing an item of metadata in a linked list
    struct _MetadataItem
    {
        friend ostest::UnitTest;

    public:
        const char* name{};

    protected:
        _MetadataItem* nextItem{};
        ostest::UnitTest& test;
        void* item{};

        _MetadataItem(ostest::UnitTest& test, const char* name, void* item);
        virtual ~_MetadataItem();
    };

    template<typename T>
    class _LinkedListIterator
    {
        friend ::ostest::Iterable<T>;

    public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = decltype(((int*)0) - ((int*)0));

    private:
        pointer nextItem{};

        explicit _LinkedListIterator(pointer firstItem) noexcept
            : nextItem(firstItem) { }

    public:
        _LinkedListIterator() = default;

        _LinkedListIterator(const _LinkedListIterator& other) noexcept
            : nextItem(other.nextItem) { }

        _LinkedListIterator& operator ++() noexcept {
            nextItem = nextItem->nextItem; return *this;
        }

        _LinkedListIterator operator ++(int) noexcept
        {
            _LinkedListIterator iter{*this};
            this->operator++();
            return iter;
        }

        bool operator ==(const _LinkedListIterator& other) const noexcept {
            return other.nextItem == nextItem;
        }
        bool operator !=(const _LinkedListIterator& other) const noexcept {
            return other.nextItem != nextItem;
        }

        reference operator *() {
            return *nextItem;
        }
    };
}

namespace ostest
{
    template<typename T>
    class Metadata final : public _ostest_internal::_MetadataItem
    {
    public:
        T value{};

    private:
        Metadata() = delete;
        Metadata& operator=(const Metadata&) = delete;
        Metadata& operator=(Metadata&&) = delete;

    public:
        /* Creates a new metadata instance. */
        template<typename Test>
        Metadata(Test& test, const char* name) :
            _ostest_internal::_MetadataItem(test, name, this)
        {
            static_assert(_ostest_internal::is_test_type<Test>::value,
                "'test' must be a valid pointer to a unit test");
            static_assert(!_ostest_internal::is_pointer_type<T>::value,
                "Metadata raw pointers will not be freed. Consider wrapping in smart pointer.");
        }
        /* Creates a new metadata instance. */
        template<typename Test, typename Y = T>
        Metadata(Test& test, const char* name, Y&& value) :
            _ostest_internal::_MetadataItem(test, name, this),
            value(_ostest_internal::forward<Y>(value))
        {
            static_assert(_ostest_internal::is_test_type<Test>::value,
                "'test' must be a valid pointer to a unit test");
        }
    };


    class Assertion
    {
        friend class _ostest_internal::_LinkedListIterator<Assertion>;
        friend class TestResult;

    private:
        bool result = true;
        Assertion* nextItem = nullptr;
        Assertion* prevItem = nullptr;

    protected:
        const bool temporary;

    public:
        const char* expression; // The assertion expression
        const char* file;       // The file in which the assertion is made
        const int line;         // The line at which the assertion is made

    public:
        /* [internal] Creates (but does not register) a new Assertion instance. */
        Assertion(const char* expression, const char* file = __FILE__,
            int line = __LINE__, bool temporary = false);

        /* [internal] Creates (but does not register) a new heap-allocated Assertion instance.
           THIS IS NOT SUPPORTED IF OSTEST IS BUILT WITH OSTEST_NO_ALLOC. */
        Assertion(const char* expression, _ostest_internal::_heapalloc_tag,
            const char* file = __FILE__, int line = __LINE__, bool temporary = false);

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

        /* Returns true if the assertion passed. False otherwise. */
        inline operator bool() const {
            return result;
        }

        /* [internal] Performs an assertion within the context of the given unit test. */
        bool evaluate(UnitTest& test, bool expression);
    };

    /* Object allowing assertion/expectation iteration. */
    template<typename T>
    class Iterable
    {
        friend TestResult;
        friend SuiteInfo;
        friend SuiteIterator getSuites() noexcept;

    public:
        using iterator = _ostest_internal::_LinkedListIterator<T>;

    private:
        T* const first;

        /* Creates a new iterable. */
        inline explicit Iterable(T* first) noexcept : first(first) { }

    public:
        inline iterator begin() noexcept {
            return iterator{first};
        }

        /* Selects the previous assertion in the collection. Returns false if none available. */
        inline iterator end() noexcept {
            return iterator{};
        }
    };

    /* Object holding the result data for a test. */
    class TestResult : private ::_ostest_internal::_LinkedList<Assertion>
    {
        friend class Assertion;

    private:
        unsigned int* refCount = nullptr;

    public:
        TestResult();
        TestResult(const TestResult& copy);
        ~TestResult();

    public:
        /* Returns true if the test succeeded. False otherwise. */
        bool succeeded() const;

        /* Gets the first assertion/expectation to fail, or nullptr if none. */
        const Assertion* getFirstFailure() const;

        /* Gets the final assertion/expectation to fail, or nullptr if none. */
        const Assertion* getFinalFailure() const;

        /* Gets the assertions and expectations made by the test. */
        inline AssertionIterator getAssertions() const {
            return AssertionIterator(this->firstItem);
        }
        /* Returns true if the test succeeded. False otherwise. */
        inline operator bool() const {
            return succeeded();
        }
    };


    class TestRunner
    {
    private:
        TestSuite& suite;
        const TestInfo& info;

    public:
        TestRunner(TestSuite& suite, const TestInfo& info)
            : suite(suite), info(info) { }

        virtual ~TestRunner() = default;

    public:
        virtual TestResult run();
    };

    class UnitTest
    {
        friend Assertion;
        friend TestRunner;
        friend _ostest_internal::_MetadataItem;

    private:
        TestResult result{};
        const TestInfo* info{};
        mutable _ostest_internal::_MetadataItem* firstUserMetadataItem{};
        mutable _ostest_internal::_MetadataItem* firstInternalMetadataItem{};

    protected:
        /* Creates (but does not register) a new Unit Test. */
        inline UnitTest(const TestInfo& info) : info(&info) { }
    public:
        virtual ~UnitTest() = default;

    protected:
        /* The Unit Test body. */
        virtual void run(TestSuite& suite) = 0;

    public:
        /* Gets TestInfo for the current Unit Test. */
        inline const TestInfo& getInfo() const { return *info; }

        /* Gets the current TestResult for the current Unit Test. */
        inline const TestResult& getResult() const { return result; }

        /* Gets the metadata with the given name, or returns nullptr if none exists. */
        template<typename T>
        Metadata<T>* getMetadata(const char* name) {
            return reinterpret_cast<Metadata<T>*>(getMetadataRaw(name));
        }
        /* Gets the metadata with the given name, or returns nullptr if none exists. */
        template<typename T>
        const Metadata<T>* getMetadata(const char* name) const {
            return reinterpret_cast<Metadata<T>*>(getMetadataRaw(name));
        }

    private:
        void addMetadata(_ostest_internal::_MetadataItem& item, bool user=true);
        void* getMetadataRaw(const char* name, bool user = true) const;
        void removeMetadata(_ostest_internal::_MetadataItem& item, bool user = true);
    };

    /* Object representing Test Suites. */
    class TestSuite
    {
        friend class TestRunner;

    public:
        virtual ~TestSuite() = 0; // Make class abstract

        // Define placement new for suites
        inline void* operator new(_ostest_internal::size_t, void* where) noexcept {
            return where;
        }

    protected:
        /* Executed before unit test run. */
        virtual void setUp() { }
        /* Executed after unit test run. */
        virtual void tearDown() { }
    };

    /* Internal interface managing object lifetimes. */
    class UnitTestWrapper
    {
        friend class TestInfo;
        friend class TestRunner;

    protected:
        virtual UnitTest& getInstance() = 0;
        virtual void newInstance() = 0;
        virtual void deleteInstance() = 0;
    };

    class SuiteUniquePtr
    {
        SuiteInfo& info;
        TestSuite& instance;

    public:
        SuiteUniquePtr(SuiteInfo& info);
        ~SuiteUniquePtr();

        SuiteUniquePtr(const SuiteUniquePtr&) = delete;
        SuiteUniquePtr(SuiteUniquePtr&&) = default;

        SuiteUniquePtr& operator =(const SuiteUniquePtr&) = delete;
        SuiteUniquePtr& operator =(SuiteUniquePtr&&) = default;

        inline TestSuite* operator ->() noexcept {
            return &instance;
        }
        inline TestSuite& operator *() noexcept {
            return instance;
        }
        inline const TestSuite* operator ->() const noexcept {
            return &instance;
        }
        inline const TestSuite& operator *() const noexcept {
            return instance;
        }
        inline TestSuite* get() noexcept {
            return &instance;
        }
        inline const TestSuite* get() const noexcept {
            return &instance;
        }
    };

    /* Class containing test suite metadata. */
    class SuiteInfo : private _ostest_internal::_StaticLinkedList<SuiteInfo>
    {
        friend TestInfo;
        friend SuiteUniquePtr;
        friend SuiteIterator;
        friend _ostest_internal::_LinkedListIterator<SuiteInfo>;
        friend SuiteIterator getSuites() noexcept;

        using ctor = TestSuite& (*)(void*);
        using dtor = void (*)(void*);

        void* ptr{};
        ctor constructor{};
        dtor destructor{};

        _ostest_internal::_LinkedList<TestInfo> _tests{};

    public:
        const char* const name{};

    private:
        SuiteInfo(void* ptr, ctor constructor, dtor destructor, const char* name);

        TestSuite& construct() {
            return constructor(ptr);
        }
        void destruct() {
            return destructor(ptr);
        }

    public:
        SuiteUniquePtr getSingletonSmartPtr() {
            return SuiteUniquePtr(*this);
        }

        TestIterator tests() {
            return TestIterator(_tests.firstItem);
        }

    public:
        template<typename T>
        static SuiteInfo& registerNew(const char* name) noexcept
        {
            alignas(alignof(T)) static char data[sizeof(T)];

            static SuiteInfo info(
                data,
                [](void* ptr) -> TestSuite& { return *(new (ptr) T()); },
                [](void* ptr) { reinterpret_cast<T*>(ptr)->T::~T(); },
                name);

            return info;
        }
    };

    /* Class containing unit test metadata. */
    class TestInfo
    {
        friend TestIterator;
        friend TestRunner;
        friend _ostest_internal::_LinkedList<TestInfo>;
        friend _ostest_internal::_LinkedListIterator<TestInfo>;
        friend _ostest_internal::_LinkedListIterator<const TestInfo>;

    private:
        ::ostest::UnitTestWrapper& wrapper;
        TestInfo* nextItem;

    public:
        const int line;         // The line of the test definition.
        const SuiteInfo& suite; // The name of the test suite containing the test.
        const char* const name; // The name of the specific test.
        const char* const file; // The file containing the test definition.

    private:
        /* Creates and registers a new TestInfo instance. */
        TestInfo(SuiteInfo& suite, const char* name,
            ::ostest::UnitTestWrapper& wrapper,
            const char* file, int line);

    public:
        // Default move constructor
        TestInfo(TestInfo&& move) noexcept = default;
        // Default copy constructor
        TestInfo(const TestInfo& copy) noexcept = default;

        /* Gets the metadata with the given name, or returns nullptr if none exists. */
        template<typename T>
        const Metadata<T>* getMetadata(const char* name) const {
            return wrapper.getInstance().getMetadata<T>(name);
        }

    public:
        /* Creates and registers a new unit test with the given details.
           Returns the new test's test info.
        */
        static TestInfo registerNew(SuiteInfo& suite, const char* name,
            UnitTestWrapper& wrapper, const char* file, int line);
    };
}


namespace _ostest_internal
{
    template<typename T>
    class _InstanceWrapper : public ::ostest::UnitTestWrapper
    {
        alignas(alignof(T)) char _data[sizeof(T)]{};
        T* _ptr{};

    public:
        _InstanceWrapper() = default;
        _InstanceWrapper(const _InstanceWrapper&) = delete;
        ~_InstanceWrapper() { deleteInstance(); }

    protected:
        T& getInstance() override
        {
            if (_ptr == nullptr) newInstance();
            return *_ptr;
        }

        void newInstance() override
        {
            if (_ptr != nullptr) deleteInstance();
            _ptr = new ((void*)_data) T();
        }

        void deleteInstance() override
        {
            if (_ptr != nullptr) return _ptr->T::~T();
        }
    };
}


// Gets the internal class name for the new unit test
#define _OSTEST_CLS_NAME(s,t) _test ## s ## t

// Gets the internal namespace for the new unit test
#define _OSTEST_NS _tests


/* [internal] Creates a new OSTest Unit Test. */
#define _OSTEST_INTERNAL(suiteClass, suiteName, testName) \
    namespace _OSTEST_NS { \
        class _OSTEST_CLS_NAME(suiteName, testName) : public ::ostest::UnitTest \
        { \
            friend class ::ostest::TestRunner; \
        private: \
            static const ostest::TestInfo info; \
            static ::_ostest_internal::_InstanceWrapper<_OSTEST_NS::_OSTEST_CLS_NAME(suiteName, testName)> _wrapper; \
        public: \
            inline _OSTEST_CLS_NAME(suiteName, testName)() noexcept : ::ostest::UnitTest(info) { } \
            inline void* operator new(::_ostest_internal::size_t, void* where) noexcept { return where; } \
        protected: \
            void testBody(suiteClass&); \
            inline void run(::ostest::TestSuite& suite) override final { testBody(reinterpret_cast<suiteClass&>(suite)); } \
        }; \
    } \
    ::_ostest_internal::_InstanceWrapper<_OSTEST_NS::_OSTEST_CLS_NAME(suiteName, testName)> _OSTEST_NS::_OSTEST_CLS_NAME(suiteName, testName)::_wrapper{}; \
    \
    const ::ostest::TestInfo _OSTEST_NS::_OSTEST_CLS_NAME(suiteName, testName)::info = ::ostest::TestInfo::registerNew( \
        ::ostest::SuiteInfo::registerNew<suiteClass>(#suiteName), \
        #testName, static_cast<::ostest::UnitTestWrapper&>(_OSTEST_NS::_OSTEST_CLS_NAME(suiteName, testName)::_wrapper), \
        __FILE__, __LINE__); \
    \
    void _OSTEST_NS::_OSTEST_CLS_NAME(suiteName, testName)::testBody([[maybe_unused]] suiteClass& suite)



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
