/* metadata-test.cpp - (c) 2018 James Renwick */
#include "common.hpp"
#include <cstring>

using namespace ostest;

namespace selftest
{
    TEST_SUITE(_MetadataSuite);

    struct test_struct
    {
        unsigned long i = 0xC0FFEE;

        test_struct() = default;
        test_struct(unsigned long i) : i(i) { }

        bool operator==(const test_struct& other) const {
            return i == other.i;
        }
    };

    TEST_EX(::selftest, _MetadataSuite, _TestNoMetadataPass)
    {
        EXPECT_EQ(getMetadata<int>(""), nullptr);
    }

    TEST_EX(::selftest, _MetadataSuite, _TestCreateMetadataPass)
    {
        static Metadata<int> zero(*this, "zero", 0);
    }

    TEST_EX(::selftest, _MetadataSuite, _TestGetMetadataPass)
    {
        static Metadata<int> value(*this, "value", 0xC0FFEE);

        EXPECT_EQ(getMetadata<int>(""), nullptr);

        ASSERT_NEQ(getMetadata<int>("value"), nullptr);
        EXPECT_EQ(getMetadata<int>("value")->value, 0xC0FFEE);
        EXPECT_ZERO(std::strcmp(getMetadata<int>("value")->name, "value"));
    }

    TEST_EX(::selftest, _MetadataSuite, _TestSetMetadataPass)
    {
        static Metadata<int> value(*this, "value", 0);
        ASSERT_NEQ(getMetadata<int>("value"), nullptr);

        EXPECT_EQ(getMetadata<int>("value")->value, 0);

        getMetadata<int>("value")->value = 0xC0FFEE;
        ASSERT_NEQ(getMetadata<int>("value"), nullptr);

        EXPECT_EQ(getMetadata<int>("value")->value, 0xC0FFEE);
    }

    TEST_EX(::selftest, _MetadataSuite, _TestMultipleMetadataPass)
    {
        using T1 = bool;
        using T2 = int;
        using T3 = test_struct;
        using T4 = unsigned int[3];

        T1 v1 = true;
        T2 v2 = 0xC0FFEE;
        T3 v3{0xDEADBEEF};

        auto f4 = [](unsigned int i) { return (i * 2) + 1; };

        static Metadata<T1> m1(*this, "m1", v1);
        static Metadata<T2> m2(*this, "m2", v2);
        static Metadata<T3> m3(*this, "m3", v3);
        static Metadata<T4> m4(*this, "m4");

        EXPECT_NEQ(getMetadata<T1>("m1"), nullptr);
        EXPECT_NEQ(getMetadata<T2>("m2"), nullptr);
        EXPECT_NEQ(getMetadata<T3>("m3"), nullptr);
        EXPECT_NEQ(getMetadata<T4>("m4"), nullptr);
        ASSERT(getResult().succeeded());

        // Assign array value
        auto& array = getMetadata<T4>("m4")->value;
        for (unsigned int i = 0; i < sizeof(T4) / sizeof(T4{}[0]); i++) {
            array[i] = f4(i);
        }

        // Mix order
        EXPECT_ZERO(std::strcmp(getMetadata<T3>("m3")->name, "m3"));
        EXPECT_ZERO(std::strcmp(getMetadata<T4>("m4")->name, "m4"));
        EXPECT_ZERO(std::strcmp(getMetadata<T1>("m1")->name, "m1"));
        EXPECT_ZERO(std::strcmp(getMetadata<T2>("m2")->name, "m2"));

        EXPECT_EQ(getMetadata<T1>("m1")->value, v1);
        EXPECT_EQ(getMetadata<T3>("m3")->value, v3);
        EXPECT_EQ(getMetadata<T2>("m2")->value, v2);

        for (unsigned int i = 0; i < sizeof(T4) / sizeof(T4{}[0]); i++)
        {
            EXPECT_ALL_OR_ASSERT(getMetadata<T4>("m4")->value[i] == f4(i));
        }
    }

    TEST_EX(::selftest, _MetadataSuite, _TestTransientMetadataOnlyPass)
    {
        {
            Metadata<bool> m1(*this, "m1", true);
            ASSERT_NEQ(getMetadata<bool>("m1"), nullptr);
            EXPECT_ZERO(std::strcmp(getMetadata<bool>("m1")->name, "m1"));
            EXPECT_EQ(getMetadata<bool>("m1")->value, true);
        }

        EXPECT_EQ(getMetadata<bool>("m1"), nullptr);
    }

    TEST_EX(::selftest, _MetadataSuite, _TestTransientMetadataPass)
    {
        using T1 = bool;
        using T2 = int;
        using T3 = test_struct;
        using T4 = int[3];

        // Test for missing metadata
        EXPECT_EQ(getInfo().getMetadata<T1>(""), nullptr);

        Metadata<T1> m1(*this, "m1");
        {
            Metadata<T2> m2(*this, "m2");
        }
        Metadata<T3> m3(*this, "m3");
        {
            Metadata<T4> m4(*this, "m4");
        }

        EXPECT_EQ(getMetadata<T2>("m2"), nullptr);
        EXPECT_EQ(getMetadata<T4>("m4"), nullptr);
        ASSERT_NEQ(getMetadata<T1>("m1"), nullptr);
        ASSERT_NEQ(getMetadata<T3>("m3"), nullptr);

        EXPECT_ZERO(std::strcmp(getMetadata<T1>("m1")->name, "m1"));
        EXPECT_ZERO(std::strcmp(getMetadata<T3>("m3")->name, "m3"));

        EXPECT_EQ(getMetadata<T1>("m1")->value, T1 {});
        EXPECT_EQ(getMetadata<T3>("m3")->value, T3 {});

        EXPECT_EQ(getInfo().getMetadata<T1>("1ab_cd"), nullptr);
    }

    TEST_EX(::selftest, _MetadataSuite, _InfoMetadataPass)
    {
        using T1 = bool;
        using T2 = int;
        using T3 = test_struct;
        using T4 = int[3];

        static Metadata<T1> m1(*this, "m1");
        static Metadata<T2> m2(*this, "m2");
        static Metadata<T3> m3(*this, "m3");
        static Metadata<T4> m4(*this, "m4");

        EXPECT_NEQ(getInfo().getMetadata<T1>("m1"), nullptr);
        EXPECT_NEQ(getInfo().getMetadata<T2>("m2"), nullptr);
        EXPECT_NEQ(getInfo().getMetadata<T3>("m3"), nullptr);
        EXPECT_NEQ(getInfo().getMetadata<T4>("m4"), nullptr);
        ASSERT(getResult().succeeded());

        // Mix order
        EXPECT_ZERO(std::strcmp(getInfo().getMetadata<T3>("m3")->name, "m3"));
        EXPECT_ZERO(std::strcmp(getInfo().getMetadata<T4>("m4")->name, "m4"));
        EXPECT_ZERO(std::strcmp(getInfo().getMetadata<T1>("m1")->name, "m1"));
        EXPECT_ZERO(std::strcmp(getInfo().getMetadata<T2>("m2")->name, "m2"));

        EXPECT_EQ(getInfo().getMetadata<T1>("m1")->value, T1 {});
        EXPECT_EQ(getInfo().getMetadata<T3>("m3")->value, T3 {});
        EXPECT_EQ(getInfo().getMetadata<T2>("m2")->value, T2 {});

        for (unsigned int i = 0; i < sizeof(T4) / sizeof(T4{}[0]); i++)
        {
            EXPECT_ALL_OR_ASSERT(getInfo().getMetadata<T4>("m4")->value[i] == 0);
        }

        // Test for missing metadata
        EXPECT_EQ(getInfo().getMetadata<T1>(""), nullptr);
    }
}


TEST_SUITE(MetadataSuite)


static bool string_ends_with(const std::string& string, const std::string& suffix)
{
    return string.size() >= suffix.size() &&
        string.compare(string.size() - suffix.size(), suffix.size(), suffix) == 0;
}

#include <cstring>

TEST(MetadataSuite, MetadataTests)
{
    for (auto& suiteInfo : getSuites())
    {
        if (std::strcmp(suiteInfo.name, "_MetadataSuite") != 0) continue;

        auto suite = suiteInfo.getSingletonSmartPtr();
        for (auto& test : suiteInfo.tests())
        {
            auto result = TestRunner(*suite, test).run();

            // If test should fail (shouldn't use metadata here)
            if (string_ends_with(test.name, "Fail"))
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
}
