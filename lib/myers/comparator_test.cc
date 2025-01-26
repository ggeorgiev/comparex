#include "gtest/gtest.h"

#include "lib/myers/comparator_test.hpp"

#include "lib/myers/classic.hpp"
#include "lib/myers/split_wavefront.hpp"

using ComparatorImplementations = ::testing::Types<
    myers::Classic<myers::Record, profile::Noop>,
    myers::SplitWavefront<myers::Record, profile::Noop>
>;

// Instantiate the test suite with all the implementations you want to test.
INSTANTIATE_TYPED_TEST_SUITE_P(Myers, ComparatorTest, ComparatorImplementations);
