#include "../../templates/socket.hpp"

#include <skynet/inet/sockaddr.hpp>
#include <skynet/inet/udp/socket.hpp>

#include <benchmark/benchmark.h>

#include "../../templates/templates.hpp"

namespace benchmarks::inet::udp {
// NOLINTBEGIN(misc-use-internal-linkage,cppcoreguidelines-avoid-magic-numbers)

BENCHMARK_T(inet_udp_socket, throughput, skynet::inet::udp::socket,
            skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                                   skynet::inet::port(8000)))
    ->Threads(2)
    ->Args({1048576, 512})
    ->Args({1048576, 128})
    ->Unit(benchmark::TimeUnit::kMicrosecond);

// NOLINTEND(misc-use-internal-linkage,cppcoreguidelines-avoid-magic-numbers)
}  // namespace benchmarks::inet::udp
