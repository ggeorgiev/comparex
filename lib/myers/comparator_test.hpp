#pragma once

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "lib/myers/record.hpp"

template <typename Comparator> class ComparatorTest : public ::testing::Test {
protected:
  std::string diffToString(const std::vector<myers::Record> &diff) {
    std::string result;
    for (auto &record : diff) {
      if (!result.empty()) {
        result += ", ";
      }
      result += record.op;
      result += " ";
      result += record.symbol;
    }
    return result;
  }
};

TYPED_TEST_SUITE_P(ComparatorTest);

// Empty vs. Empty
TYPED_TEST_P(ComparatorTest, emptyVsEmpty) {
  std::string a;
  std::string b;

  TypeParam algorithm;
  auto diff = algorithm.compare(a, b);

  // Both strings are empty -> no operations expected
  ASSERT_TRUE(diff.empty()) << "Expected empty diff, got: " << this->diffToString(diff);
}

// 3. Empty vs. Non-Empty
TYPED_TEST_P(ComparatorTest, emptyVsNonEmpty) {
  std::string a;
  std::string b = "ABC";

  TypeParam algorithm;
  auto diff = algorithm.compare(a, b);

  // We expect to only have insertions if a is empty
  // e.g. "I A", "I B", "I C"
  ASSERT_EQ(diff.size(), b.size()) << "Diff is: " << this->diffToString(diff);

  // Check each operation is an insert
  for (size_t i = 0; i < diff.size(); ++i) {
    EXPECT_EQ(diff[i].op, '+') << "Expected insert for record: " << this->diffToString(diff);
    EXPECT_EQ(diff[i].symbol, b[i]) << "Incorrect symbol in diff record";
  }
}

// 4. Non-Empty vs. Empty
TYPED_TEST_P(ComparatorTest, nonEmptyVsEmpty) {
  std::string a = "ABC";
  std::string b;

  TypeParam algorithm;
  auto diff = algorithm.compare(a, b);

  // We expect to only have deletions if b is empty
  ASSERT_EQ(diff.size(), a.size()) << "Diff is: " << this->diffToString(diff);

  // Check each operation is a delete
  for (size_t i = 0; i < diff.size(); ++i) {
    EXPECT_EQ(diff[i].op, '-') << "Expected delete for record: " << this->diffToString(diff);
    EXPECT_EQ(diff[i].symbol, a[i]) << "Incorrect symbol in diff record";
  }
}

// 5. Identical strings
TYPED_TEST_P(ComparatorTest, identicalStrings) {
  std::string a = "ABCABC";
  std::string b = "ABCABC";

  TypeParam algorithm;
  auto diff = algorithm.compare(a, b);

  // Check each operation is keep
  for (size_t i = 0; i < diff.size(); ++i) {
    EXPECT_EQ(diff[i].op, ' ') << "Expected delete for record: " << this->diffToString(diff);
    EXPECT_EQ(diff[i].symbol, a[i]) << "Incorrect symbol in diff record";
  }
}

// Front character difference
TYPED_TEST_P(ComparatorTest, frontCharDifference) {
  std::string a = "XAB";
  std::string b = "YAB";

  TypeParam algorithm;
  auto diff = algorithm.compare(a, b);

  EXPECT_EQ(this->diffToString(diff), "- X, + Y,   A,   B");
}

// Back character difference
TYPED_TEST_P(ComparatorTest, backCharDifference) {
  std::string a = "ABX";
  std::string b = "ABY";

  TypeParam algorithm;
  auto diff = algorithm.compare(a, b);

  EXPECT_EQ(this->diffToString(diff), "  A,   B, - X, + Y");
}

// Middle character difference
TYPED_TEST_P(ComparatorTest, middleCharDifference) {
  std::string a = "AXB";
  std::string b = "AYB";

  TypeParam algorithm;
  auto diff = algorithm.compare(a, b);

  EXPECT_EQ(this->diffToString(diff), "  A, - X, + Y,   B");
}

// Completely different strings of the same length
TYPED_TEST_P(ComparatorTest, sameLengthDifferentStrings) {
  std::string a = "AAAA";
  std::string b = "BBBB";

  TypeParam algorithm;
  auto diff = algorithm.compare(a, b);

  EXPECT_EQ(this->diffToString(diff), "- A, - A, - A, - A, + B, + B, + B, + B");
}

// Everywhere character difference
TYPED_TEST_P(ComparatorTest, everywhereCharDifference) {
  std::string a = "XAXBX";
  std::string b = "YAYBY";

  TypeParam algorithm;
  auto diff = algorithm.compare(a, b);

  EXPECT_EQ(this->diffToString(diff), "- X, + Y,   A, - X, + Y,   B, - X, + Y");
}

// Front substring
TYPED_TEST_P(ComparatorTest, frontSubstring) {
  std::string a = "ABCDEF";
  std::string b = "ABC";

  TypeParam algorithm;

  auto diff1 = algorithm.compare(a, b);
  EXPECT_EQ(this->diffToString(diff1), "  A,   B,   C, - D, - E, - F");

  auto diff2 = algorithm.compare(b, a);
  EXPECT_EQ(this->diffToString(diff2), "  A,   B,   C, + D, + E, + F");
}

// Back substring
TYPED_TEST_P(ComparatorTest, backSubstring) {
  std::string a = "ABCDEF";
  std::string b = "DEF";

  TypeParam algorithm;
  auto diff1 = algorithm.compare(a, b);
  EXPECT_EQ(this->diffToString(diff1), "- A, - B, - C,   D,   E,   F");

  auto diff2 = algorithm.compare(b, a);
  EXPECT_EQ(this->diffToString(diff2), "+ A, + B, + C,   D,   E,   F");
}

// Sanity check
TYPED_TEST_P(ComparatorTest, sanity) {
  std::string a = "ABCABBA";
  std::string b = "CAB";

  TypeParam algorithm;
  auto diff = algorithm.compare(a, b);

  EXPECT_EQ(this->diffToString(diff), "- A, - B,   C,   A, - B,   B, - A");
}

// Sanity check
TYPED_TEST_P(ComparatorTest, classic) {
  std::string a = "ABCABBA";
  std::string b = "CBABAC";

  TypeParam algorithm;
  auto diff = algorithm.compare(a, b);

  EXPECT_EQ(this->diffToString(diff), "- A, - B,   C, + B,   A, - B,   B,   A, + C");
}

// Register all tests in this test suite
REGISTER_TYPED_TEST_SUITE_P(ComparatorTest, emptyVsEmpty, emptyVsNonEmpty, nonEmptyVsEmpty,
                            identicalStrings, frontCharDifference, everywhereCharDifference,
                            middleCharDifference, backCharDifference, sameLengthDifferentStrings,
                            frontSubstring, backSubstring, sanity, classic);
