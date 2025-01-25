#include <iostream>

#include "benchmark/benchmark.h"
#include "lib/myers/classic.hpp"
#include "lib/profile/track.hpp"

static void BM_Myers(benchmark::State& state) {
  std::string a = "axc";
  std::string b = "ayc";

  for (int i = 0; i < 6; i++) {
    a += a;
    b += b;
  }

  myers::Classic classic;

  for (auto s : state) {
    classic.compare(a, b);
  }
}

static void BM_Track_Myers(benchmark::State& state) {
  std::string a = "axc";
  std::string b = "ayc";

  for (int i = 0; i < 6; i++) {
    a += a;
    b += b;
  }

  myers::Classic<myers::Record, profile::Track> classic;

  for (auto s : state) {
    classic.compare(a, b);
  }
}

static void TR_Myers(benchmark::State& state) {
  std::string a = "axc";
  std::string b = "ayc";

  for (int i = 0; i < 6; i++) {
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
BENCHMARK(BM_Myers);
BENCHMARK(BM_Track_Myers);

BENCHMARK(TR_Myers)->Iterations(1);

// Run the benchmark
BENCHMARK_MAIN();