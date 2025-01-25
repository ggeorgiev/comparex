#pragma once

#include <gtest/gtest.h>
#include <string>

// Define a templated test fixture
template <typename Comparator>
class ComparatorTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(ComparatorTest);

// Define the tests you want to run for each type
TYPED_TEST_P(ComparatorTest, sanity)
{
    std::string a = "ABCABBA";
    std::string b = "CAB";

    // Create the algorithm instance from the current TypeParam
    TypeParam algorithm;
    auto diff = algorithm.compare(a, b);

    // Example assertion
    ASSERT_EQ(diff.size(), 7);
}

// Register tests for this test suite
REGISTER_TYPED_TEST_SUITE_P(ComparatorTest, sanity);
