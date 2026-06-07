#include "../../templates/socket.hpp"

#include <skynet/unix/sockaddr.hpp>
#include <skynet/unix/stream/socket.hpp>

#include <benchmark/benchmark.h>

#include "../../templates/templates.hpp"

namespace benchmarks::unix::stream {
// NOLINTBEGIN(misc-use-internal-linkage,cppcoreguidelines-avoid-magic-numbers)

BENCHMARK_T(unix_stream_socket, nonblock_handshake,
            skynet::unix::stream::socket,
            skynet::unix::sockaddr("BM_unix_stream_socket_nonblock_handshake"))
    ->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_T(unix_stream_socket, throughput, skynet::unix::stream::socket,
            skynet::unix::sockaddr("BM_unix_stream_socket_throughput"))
    ->Threads(2)
    ->Args({1048576, 512})
    ->Args({1048576, 128})
    ->Unit(benchmark::TimeUnit::kMicrosecond);

// NOLINTEND(misc-use-internal-linkage,cppcoreguidelines-avoid-magic-numbers)
}  // namespace benchmarks::unix::stream
