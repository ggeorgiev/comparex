#include "gtest/gtest.h"

#include "lib/myers/comparator_test.hpp"

#include "lib/myers/classic.hpp"
#include "lib/myers/split_wavefront.hpp"

using ComparatorImplementations =
    ::testing::Types<myers::Classic<std::string, myers::Record, profile::Noop>,
                     myers::SplitWavefront<std::string, myers::Record, profile::Noop>>;

// Instantiate the test suite with all the implementations you want to test.
INSTANTIATE_TYPED_TEST_SUITE_P(Myers, ComparatorTest, ComparatorImplementations);

#include <string>

class NoSizeString : public std::string
{
public:
    using std::string::string; // Inherit constructors

    // Delete size()
    size_type size() const = delete;
    size_type length() const = delete;
};


TEST(Limitations, no_size) {
    myers::Classic<std::string, myers::Record, profile::Noop, uint32_t> comparator;
    comparator.compare("", "");
}