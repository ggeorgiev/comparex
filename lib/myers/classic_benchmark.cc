#include "benchmark/benchmark.h"

#include "lib/myers/classic.hpp"

static void BM_myers(benchmark::State& state) {
  for (auto s : state) {
    std::string a = "abc";
    std::string b = "ac";
    myers::compare(a, b);
  }
}

// Register the function as a benchmark
BENCHMARK(BM_myers);

// Run the benchmark
BENCHMARK_MAIN();