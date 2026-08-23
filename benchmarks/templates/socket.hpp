#pragma once

#include <skynet/base_sockaddr.hpp>
#include <skynet/base_socket.hpp>
#include <skynet/socket.hpp>

#include <atomic>

#include <benchmark/benchmark.h>

namespace benchmarks::templates {

template <typename Socket, skynet::concepts::sockaddr Sockaddr>
constexpr void nonblock_handshake(benchmark::State & state,
                                  Sockaddr const & sockaddr) noexcept
  requires(skynet::concepts::stream_socket<Socket, Sockaddr>)
{
  // NOLINTNEXTLINE(readability-identifier-length)
  for (auto const _ : state) {
    Socket server;
    server.set_option(skynet::options::nonblock(true));
    server.set_option(skynet::options::reuseaddr(true));
    server.set_option(skynet::options::reuseport(true));
    server.bind(sockaddr);
    server.listen(1);

    Socket client;
    client.set_option(skynet::options::nonblock(true));
    client.set_option(skynet::options::reuseaddr(true));
    client.set_option(skynet::options::reuseport(true));

    while (client.connect(sockaddr) == skynet::connect_status::kRefused) {
    }
    Socket peer(skynet::uninitialized);
    while (server.accept(peer) != skynet::accept_status::kSuccess) {
    }
  }
}

template <typename Socket, skynet::concepts::sockaddr Sockaddr>
constexpr void throughput(benchmark::State & state,
                          Sockaddr const & sockaddr) noexcept
  requires(skynet::concepts::stream_socket<Socket, Sockaddr>)
{
  auto const data_size   = static_cast<std::size_t>(state.range(0));
  auto const buffer_size = static_cast<std::size_t>(state.range(1));

  Socket socket;
  socket.set_option(skynet::options::nonblock(true));
  socket.set_option(skynet::options::reuseaddr(true));
  socket.set_option(skynet::options::reuseport(true));

  std::vector<std::byte> buffer(buffer_size);
  if (state.thread_index() == 0) {
    socket.bind(sockaddr);
    socket.listen(1);

    Socket peer(skynet::uninitialized);
    while (socket.accept(peer) != skynet::accept_status::kSuccess) {
    }

    // NOLINTNEXTLINE(readability-identifier-length)
    for (auto const _ : state) {
      std::size_t received = 0;
      while (received < data_size) {
        received += peer.recv(buffer);
      }
    }
  } else {
    while (socket.connect(sockaddr) != skynet::connect_status::kSuccess) {
    }

    // NOLINTNEXTLINE(readability-identifier-length)
    for (auto const _ : state) {
      std::size_t sent = 0;
      while (sent < data_size) {
        sent += socket.send(buffer);
      }
    }
  }
}

template <typename Socket, skynet::concepts::sockaddr Sockaddr>
constexpr void throughput(benchmark::State & state,
                          Sockaddr const & sockaddr) noexcept
  requires(skynet::concepts::dgram_socket<Socket, Sockaddr>)
{
  static std::atomic_bool sender_done;

  auto const data_size   = static_cast<std::size_t>(state.range(0));
  auto const buffer_size = static_cast<std::size_t>(state.range(1));

  Socket socket;
  socket.set_option(skynet::options::nonblock(true));
  socket.set_option(skynet::options::reuseaddr(true));
  socket.set_option(skynet::options::reuseport(true));

  std::vector<std::byte> buffer(buffer_size);
  if (state.thread_index() == 0) {
    socket.bind(sockaddr);

    std::size_t loss = 0;
    // NOLINTNEXTLINE(readability-identifier-length)
    for (auto const _ : state) {
      std::size_t received = 0;
      while (received < data_size) {
        std::size_t const _received = socket.recv(buffer);
        if (_received == 0 && sender_done.load(std::memory_order::acquire)) {
          break;
        }
        received += _received;
      }
      loss += (data_size - received);
    }
    state.counters["loss"] = benchmark::Counter(
        static_cast<double>(loss * static_cast<std::size_t>(state.threads())),
        benchmark::Counter::kAvgIterations);
  } else {
    while (socket.connect(sockaddr) != skynet::connect_status::kSuccess) {
    }

    // NOLINTNEXTLINE(readability-identifier-length)
    for (auto const _ : state) {
      sender_done.store(false, std::memory_order::release);
      std::size_t sent = 0;
      while (sent < data_size) {
        sent += socket.send(buffer);
      }
      sender_done.store(true, std::memory_order::release);
    }
  }

  sender_done.store(false, std::memory_order::release);
}

}  // namespace benchmarks::templates
