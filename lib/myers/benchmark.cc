#include <iostream>

#include "benchmark/benchmark.h"
#include "lib/myers/classic.hpp"
#include "lib/myers/split_wavefront.hpp"
#include "lib/profile/track.hpp"

template <class C> void BM_Compare(benchmark::State &state) {
  std::string a = "axc";
  std::string b = "ayc";

  for (int i = 0; i < 4; i++) {
    a += a + a;
    b += b;
  }

  C comparer;

  for (auto s : state) {
    comparer.compare(a, b);
  }
}

static void TR_Myers(benchmark::State &state) {
  std::string a = "axc";
  std::string b = "ayc";

  for (int i = 0; i < 4; i++) {
    a += a + a;
    b += b;
  }

  myers::Classic<std::string, myers::Record, profile::Track, std::size_t> classic;
  for (auto s : state) {
    classic.compare(a, b);
  }

  state.counters["memory"] = classic.memory();
}

// Register the function as a benchmark
BENCHMARK_TEMPLATE(BM_Compare, myers::Classic<std::string, myers::Record, profile::Noop, uint32_t>);
BENCHMARK_TEMPLATE(BM_Compare, myers::Classic<std::string, myers::Record, profile::Noop, uint64_t>);
BENCHMARK_TEMPLATE(BM_Compare, myers::Classic<std::string, myers::Record, profile::Track, uint32_t>);
BENCHMARK_TEMPLATE(BM_Compare, myers::SplitWavefront<std::string, myers::Record, profile::Noop, uint32_t>);

BENCHMARK(TR_Myers)->Iterations(1);

// Run the benchmark
BENCHMARK_MAIN();