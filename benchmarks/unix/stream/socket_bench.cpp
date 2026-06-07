#include "../../templates/socket.hpp"

#include <skynet/unix/sockaddr.hpp>
#include <skynet/unix/stream/socket.hpp>

#include <benchmark/benchmark.h>

#include "../../templates/templates.hpp"

namespace benchmarks::unix::stream {

BENCHMARK_T(unix_stream_socket, nonblock_handshake, unix::stream::socket,
            unix::sockaddr("BM_unix_stream_socket_nonblock_handshake"))
    ->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_T(unix_stream_socket, throughput, unix::stream::socket,
            unix::sockaddr("BM_unix_stream_socket_throughput"))
    ->Threads(2)
    ->Args({1048576, 512})
    ->Args({1048576, 128})
    ->Unit(benchmark::TimeUnit::kMicrosecond);

}  // namespace benchmarks::unix::stream
