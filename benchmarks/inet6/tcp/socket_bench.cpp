#include "../../templates/socket.hpp"

#include <skynet/inet6/sockaddr.hpp>
#include <skynet/inet6/tcp/socket.hpp>

#include <benchmark/benchmark.h>

#include "../../templates/templates.hpp"

namespace benchmarks::inet6::tcp {

BENCHMARK_T(inet6_tcp, nonblock_handshake, skynet::inet6::tcp::socket,
            skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                                    skynet::inet6::port(8000)))
    ->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_T(inet6_tcp, stream_throughput, skynet::inet6::tcp::socket,
            skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                                    skynet::inet6::port(8000)))
    ->Threads(2)
    ->Args({1048576, 512})
    ->Args({1048576, 128})
    ->Unit(benchmark::TimeUnit::kMicrosecond);

}  // namespace benchmarks::inet6::tcp
