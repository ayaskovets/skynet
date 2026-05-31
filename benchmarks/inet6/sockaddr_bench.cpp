#include <skynet/inet6/sockaddr.hpp>

#include <benchmark/benchmark.h>

namespace benchmarks::net::inet6 {
// NOLINTBEGIN(misc-use-internal-linkage)

constexpr void BM_net_inet6_from_bytes(benchmark::State & state) noexcept {
  std::array<std::byte, 16> const bytes{
      std::byte(255), std::byte(255), std::byte(255), std::byte(255),
      std::byte(255), std::byte(255), std::byte(255), std::byte(255),
      std::byte(255), std::byte(255), std::byte(255), std::byte(255),
      std::byte(255), std::byte(255), std::byte(255), std::byte(255)};
  // NOLINTNEXTLINE(readability-identifier-length)
  for (auto const _ : state) {
    benchmark::DoNotOptimize(skynet::inet6::sockaddr(
        skynet::inet6::ip(skynet::network_byte_order_t{}, std::span(bytes)),
        skynet::inet6::port(0)));
  }
}
BENCHMARK(BM_net_inet6_from_bytes);

constexpr void BM_net_inet6_from_string(benchmark::State & state) noexcept {
  std::string const string("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
  // NOLINTNEXTLINE(readability-identifier-length)
  for (auto const _ : state) {
    benchmark::DoNotOptimize(skynet::inet6::sockaddr(skynet::inet6::ip(string),
                                                     skynet::inet6::port(0)));
  }
}
BENCHMARK(BM_net_inet6_from_string);

constexpr void BM_net_inet6_to_string(benchmark::State & state) noexcept {
  skynet::inet6::sockaddr const sockaddr(
      skynet::inet6::ip("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"),
      skynet::inet6::port(0));
  // NOLINTNEXTLINE(readability-identifier-length)
  for (auto const _ : state) {
    benchmark::DoNotOptimize(sockaddr.to_string());
  }
}
BENCHMARK(BM_net_inet6_to_string);

// NOLINTEND(misc-use-internal-linkage)
}  // namespace benchmarks::net::inet6
