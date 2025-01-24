#include "gtest/gtest.h"

#include "lib/myers/classic.hpp"

TEST(hello_world_test, hello) {
    std::string a = "ABCABBA";
    std::string b = "CAB";

    for (int i = 0; i < 10; ++i) {
        auto diff = myers::compare(a, b);
        ASSERT_EQ(diff.size(), 7); 
    }
}
