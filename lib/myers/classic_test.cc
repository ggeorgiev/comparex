#include "gtest/gtest.h"

#include "lib/myers/classic.hpp"

TEST(hello_world_test, hello) {
    std::string a = "ABCABBA";
    std::string b = "CAB";

    myers::Classic classic;
    auto diff = classic.compare(a, b);
    ASSERT_EQ(diff.size(), 7); 
}
