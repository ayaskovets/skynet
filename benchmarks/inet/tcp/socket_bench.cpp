#include "../../templates/socket.hpp"

#include <skynet/inet/sockaddr.hpp>
#include <skynet/inet/tcp/socket.hpp>

#include <benchmark/benchmark.h>

#include "../../templates/templates.hpp"

namespace benchmarks::inet::tcp {

BENCHMARK_T(inet_tcp_socket, nonblock_handshake, skynet::inet::tcp::socket,
            skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                                   skynet::inet::port(8000)))
    ->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_T(inet_tcp_socket, throughput, skynet::inet::tcp::socket,
            skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                                   skynet::inet::port(8000)))
    ->Threads(2)
    ->Args({1048576, 512})
    ->Args({1048576, 128})
    ->Unit(benchmark::TimeUnit::kMicrosecond);

}  // namespace benchmarks::inet::tcp
