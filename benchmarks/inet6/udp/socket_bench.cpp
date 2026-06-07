#include "../../templates/socket.hpp"

#include <skynet/inet6/sockaddr.hpp>
#include <skynet/inet6/udp/socket.hpp>

#include <benchmark/benchmark.h>

#include "../../templates/templates.hpp"

namespace benchmarks::inet6::udp {

BENCHMARK_T(inet6_udp_socket, throughput, skynet::inet6::udp::socket,
            skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                                    skynet::inet6::port(8000)))
    ->Threads(2)
    ->Args({1048576, 512})
    ->Args({1048576, 128})
    ->Unit(benchmark::TimeUnit::kMicrosecond);

}  // namespace benchmarks::inet6::udp
