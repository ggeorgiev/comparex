#include "gtest/gtest.h"
#include "lib/myers/algorithm.hpp"

TEST(hello_world_test, hello) {
  std::string foo;
  std::string bar;

  auto result = compare<std::string>(foo, bar);
  EXPECT_EQ(result, false);
};
