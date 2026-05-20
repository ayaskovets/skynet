#include <skynet/dns.hpp>

#include <benchmark/benchmark.h>

namespace benchmarks {
// NOLINTBEGIN(misc-use-internal-linkage)

constexpr void BM_dns_resolve_localhost(benchmark::State & state) noexcept {
  // NOLINTNEXTLINE(readability-identifier-length)
  for (auto const _ : state) {
    benchmark::DoNotOptimize(skynet::resolve(
        "localhost", skynet::family::kUnspecified, skynet::protocol::kTcp));
  }
}
BENCHMARK(BM_dns_resolve_localhost)->Unit(benchmark::kMicrosecond);

constexpr void BM_dns_resolve_google(benchmark::State & state) noexcept {
  try {
    skynet::resolve("google.com", skynet::family::kUnspecified,
                    skynet::protocol::kTcp);
  } catch (std::exception const & e) {
    state.SkipWithMessage(e.what());
  }

  // NOLINTNEXTLINE(readability-identifier-length)
  for (auto const _ : state) {
    benchmark::DoNotOptimize(skynet::resolve(
        "google.com", skynet::family::kUnspecified, skynet::protocol::kTcp));
  }
}
BENCHMARK(BM_dns_resolve_google)->Unit(benchmark::kMicrosecond);

// NOLINTEND(misc-use-internal-linkage)
}  // namespace benchmarks
