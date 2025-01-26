#include <iostream>

#include "benchmark/benchmark.h"
#include "lib/myers/classic.hpp"
#include "lib/profile/track.hpp"
#include "lib/myers/split_wavefront.hpp"

template <class C> void BM_Compare(benchmark::State& state) {
  std::string a = "axc";
  std::string b = "ayc";

  for (int i = 0; i < 6; i++) {
    a += a;
    b += b;
  }

  C comparer;

  for (auto s : state) {
    comparer.compare(a, b);
  }
}

static void TR_Myers(benchmark::State& state) {
  std::string a = "axc";
  std::string b = "ayc";

  for (int i = 0; i < 0; i++) {
    a += a;
    b += b;
  }

  myers::Classic<myers::Record, profile::Track> classic;
  for (auto s : state) {
    classic.compare(a, b);
  }

  state.counters["memory"] = classic.memory();
}

// Register the function as a benchmark
BENCHMARK_TEMPLATE(BM_Compare, myers::Classic<myers::Record, profile::Noop>);
BENCHMARK_TEMPLATE(BM_Compare, myers::Classic<myers::Record, profile::Track>);
BENCHMARK_TEMPLATE(BM_Compare, myers::SplitWavefront<myers::Record, profile::Noop>);
BENCHMARK_TEMPLATE(BM_Compare, myers::SplitWavefront<myers::Record, profile::Track>);

BENCHMARK(TR_Myers)->Iterations(1);

// Run the benchmark
BENCHMARK_MAIN();