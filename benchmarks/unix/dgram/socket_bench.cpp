#include "../../templates/socket.hpp"

#include <skynet/unix/dgram/socket.hpp>
#include <skynet/unix/sockaddr.hpp>

#include <benchmark/benchmark.h>

#include "../../templates/templates.hpp"

namespace benchmarks::unix::dgram {
// NOLINTBEGIN(misc-use-internal-linkage,cppcoreguidelines-avoid-magic-numbers)

BENCHMARK_T(unix_dgram_socket, throughput, skynet::unix::dgram::socket,
            skynet::unix::sockaddr("BM_unix_dgram_socket_dgram_throughput"))
    ->Threads(2)
    ->Args({1048576, 512})
    ->Args({1048576, 128})
    ->Unit(benchmark::TimeUnit::kMicrosecond);

// NOLINTEND(misc-use-internal-linkage,cppcoreguidelines-avoid-magic-numbers)
}  // namespace benchmarks::unix::dgram
