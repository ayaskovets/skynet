#include <skynet/inet/sockaddr.hpp>

#include <benchmark/benchmark.h>

namespace benchmarks::inet {
// NOLINTBEGIN(misc-use-internal-linkage)

constexpr void BM_net_inet_sockaddr_from_bytes(
    benchmark::State & state) noexcept {
  std::uint32_t const bytes(0x7f000001);
  // NOLINTNEXTLINE(readability-identifier-length)
  for (auto const _ : state) {
    benchmark::DoNotOptimize(
        skynet::inet::sockaddr(skynet::inet::ip(bytes), skynet::inet::port(0)));
  }
}
BENCHMARK(BM_net_inet_sockaddr_from_bytes);

constexpr void BM_net_inet_sockaddr_from_string(
    benchmark::State & state) noexcept {
  std::string const string("127.0.0.1");
  // NOLINTNEXTLINE(readability-identifier-length)
  for (auto const _ : state) {
    benchmark::DoNotOptimize(skynet::inet::sockaddr(skynet::inet::ip(string),
                                                    skynet::inet::port(0)));
  }
}
BENCHMARK(BM_net_inet_sockaddr_from_string);

constexpr void BM_net_inet_sockaddr_to_string(
    benchmark::State & state) noexcept {
  skynet::inet::sockaddr const sockaddr(skynet::inet::ip("127.0.0.1"),
                                        skynet::inet::port(0));
  // NOLINTNEXTLINE(readability-identifier-length)
  for (auto const _ : state) {
    benchmark::DoNotOptimize(sockaddr.to_string());
  }
}
BENCHMARK(BM_net_inet_sockaddr_to_string);

// NOLINTEND(misc-use-internal-linkage)
}  // namespace benchmarks::inet
