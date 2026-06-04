#include <skynet/unix/sockaddr.hpp>

#include <benchmark/benchmark.h>

namespace benchmarks::unix {
// NOLINTBEGIN(misc-use-internal-linkage)

constexpr void BM_unix_from_string(benchmark::State & state) noexcept {
  std::string const string("BM_unix_from_string_unix_sockaddr");
  // NOLINTNEXTLINE(readability-identifier-length)
  for (auto const _ : state) {
    benchmark::DoNotOptimize(skynet::unix::sockaddr(string));
  }
}
BENCHMARK(BM_unix_from_string);

constexpr void BM_unix_to_string(benchmark::State & state) noexcept {
  skynet::unix::sockaddr const sockaddr("BM_unix_from_string_unix_sockaddr");
  // NOLINTNEXTLINE(readability-identifier-length)
  for (auto const _ : state) {
    benchmark::DoNotOptimize(sockaddr.to_string());
  }
}
BENCHMARK(BM_unix_to_string);

// NOLINTEND(misc-use-internal-linkage)
}  // namespace benchmarks::unix
