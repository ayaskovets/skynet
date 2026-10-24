#pragma once

#include <skynet/base_socket.hpp>
#include <skynet/socket.hpp>

#include <thread>

#include <gtest/gtest.h>

namespace tests::unit::templates {

template <typename Socket, skynet::concepts::sockaddr Sockaddr>
constexpr void unconnected_send_receive(Sockaddr const &) noexcept
  requires(skynet::concepts::stream_socket<Socket, Sockaddr>)
{
  std::vector<std::byte> const data{std::byte(1), std::byte(2), std::byte(3)};

  Socket socket;
  socket.set_option(skynet::options::reuseaddr(true));
  socket.set_option(skynet::options::reuseport(true));

  EXPECT_ANY_THROW(socket.send(data));
  std::vector<std::byte> buffer(data.size());
  EXPECT_ANY_THROW(socket.recv(buffer));
}

template <typename Socket, skynet::concepts::sockaddr Sockaddr>
constexpr void unconnected_send_receive(Sockaddr const & sockaddr) noexcept
  requires(skynet::concepts::dgram_socket<Socket, Sockaddr>)
{
  std::vector<std::byte> const data{std::byte(1), std::byte(2), std::byte(3)};

  Socket socket;
  socket.set_option(skynet::options::nonblock(true));
  socket.set_option(skynet::options::reuseaddr(true));
  socket.set_option(skynet::options::reuseport(true));

  EXPECT_EQ(socket.sendto(data, sockaddr), data.size());
  std::vector<std::byte> buffer(data.size());
  EXPECT_EQ(socket.recv(buffer), Socket::kWouldBlock);
}

template <typename Socket, skynet::concepts::sockaddr Sockaddr>
constexpr void bind(Sockaddr const & sockaddr) noexcept
  requires(skynet::concepts::socket<Socket, Sockaddr>)
{
  Socket socket;
  socket.set_option(skynet::options::reuseaddr(true));
  socket.set_option(skynet::options::reuseport(true));
  EXPECT_EQ(socket.bind(sockaddr), skynet::bind_status::kSuccess);
  EXPECT_ANY_THROW(socket.bind(sockaddr));

  Socket other;
  EXPECT_EQ(other.bind(sockaddr), skynet::bind_status::kInUse);
  other.set_option(skynet::options::reuseaddr(true));
  other.set_option(skynet::options::reuseport(true));
  EXPECT_EQ(other.bind(sockaddr), skynet::bind_status::kSuccess);
}

template <typename Socket, skynet::concepts::sockaddr Sockaddr>
constexpr void connect(Sockaddr const & sockaddr,
                       auto const & connect_status) noexcept
  requires(skynet::concepts::socket<Socket, Sockaddr>)
{
  Socket socket;
  socket.set_option(skynet::options::reuseaddr(true));
  socket.set_option(skynet::options::reuseport(true));

  EXPECT_EQ(socket.connect(sockaddr), connect_status);
}

template <typename Socket, skynet::concepts::sockaddr Sockaddr>
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
constexpr void sockaddrs(Sockaddr const & sockaddr,
                         Sockaddr const & peer_sockaddr,
                         auto const & connect_status) noexcept
  requires(skynet::concepts::socket<Socket, Sockaddr>)
{
  Socket socket;
  socket.set_option(skynet::options::nonblock(true));
  socket.set_option(skynet::options::reuseaddr(true));
  socket.set_option(skynet::options::reuseport(true));

  auto out_sockaddr = sockaddr;
  EXPECT_NO_THROW((socket.getsockname(out_sockaddr)));
  EXPECT_NE((socket.getsockname(out_sockaddr), out_sockaddr), sockaddr);
  socket.bind(sockaddr);
  EXPECT_EQ((socket.getsockname(out_sockaddr), out_sockaddr), sockaddr);
  EXPECT_ANY_THROW(socket.getpeername(out_sockaddr));

  out_sockaddr = sockaddr;
  EXPECT_NE(out_sockaddr, peer_sockaddr);
  EXPECT_EQ(socket.connect(peer_sockaddr), connect_status);
  if (connect_status == skynet::connect_status::kSuccess) {
    EXPECT_EQ((socket.getpeername(out_sockaddr), out_sockaddr), peer_sockaddr);
  } else {
    EXPECT_ANY_THROW(socket.getpeername(out_sockaddr));
  }
}

template <typename Socket, skynet::concepts::sockaddr Sockaddr>
constexpr void unconnected_accept(Sockaddr const & sockaddr) noexcept
  requires(skynet::concepts::stream_socket<Socket, Sockaddr>)
{
  Socket socket;
  socket.set_option(skynet::options::nonblock(true));
  socket.set_option(skynet::options::reuseaddr(true));
  socket.set_option(skynet::options::reuseport(true));
  socket.bind(sockaddr);

  Socket peer(skynet::uninitialized);
  EXPECT_ANY_THROW(socket.accept(peer));
  socket.listen(1);
  EXPECT_EQ(socket.accept(peer), skynet::accept_status::kEmptyQueue);
}

template <typename Socket, skynet::concepts::sockaddr Sockaddr>
constexpr void blocking_handshake(Sockaddr const & sockaddr) noexcept
  requires(skynet::concepts::stream_socket<Socket, Sockaddr>)
{
  std::vector<std::byte> const data{std::byte(1), std::byte(2), std::byte(3)};

  Socket server;
  server.set_option(skynet::options::reuseaddr(true));
  server.set_option(skynet::options::reuseport(true));
  server.bind(sockaddr);
  server.listen(1);

  std::jthread const server_thread([&server, &data] -> void {
    Socket peer;
    EXPECT_EQ(server.accept(peer), skynet::accept_status::kSuccess);

    std::vector<std::byte> buffer(data.size());
    EXPECT_EQ(peer.recv(buffer), data.size());
    EXPECT_EQ(buffer, data);
    EXPECT_EQ(peer.send(data), data.size());
  });

  Socket client;
  client.set_option(skynet::options::reuseaddr(true));
  client.set_option(skynet::options::reuseport(true));

  EXPECT_EQ(client.connect(sockaddr), skynet::connect_status::kSuccess);
  EXPECT_EQ(client.send(data), data.size());
  std::vector<std::byte> buffer(data.size());
  EXPECT_EQ(client.recv(buffer), data.size());
  EXPECT_EQ(buffer, data);
}

template <typename Socket, skynet::concepts::sockaddr Sockaddr>
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
constexpr void blocking_handshake(Sockaddr const & server_sockaddr,
                                  Sockaddr const & client_sockaddr) noexcept
  requires(skynet::concepts::dgram_socket<Socket, Sockaddr>)
{
  std::vector<std::byte> const data{std::byte(1), std::byte(2), std::byte(3)};

  Socket server;
  server.set_option(skynet::options::reuseaddr(true));
  server.set_option(skynet::options::reuseport(true));
  server.bind(server_sockaddr);

  std::jthread const server_thread([&server, &client_sockaddr, &data] -> void {
    auto peer(client_sockaddr);
    std::vector<std::byte> buffer(data.size());
    EXPECT_EQ(server.recvfrom(buffer, peer), data.size());
    EXPECT_EQ(buffer, data);
    EXPECT_EQ(peer, client_sockaddr);
    EXPECT_EQ(server.sendto(data, peer), data.size());

    EXPECT_EQ(server.recv(buffer), data.size());
    EXPECT_EQ(server.connect(client_sockaddr),
              skynet::connect_status::kSuccess);
    EXPECT_EQ(server.send(data), data.size());
  });

  auto peer(server_sockaddr);
  std::vector<std::byte> buffer(data.size());

  Socket client;
  client.set_option(skynet::options::reuseaddr(true));
  client.set_option(skynet::options::reuseport(true));
  client.bind(client_sockaddr);

  EXPECT_EQ(client.sendto(data, server_sockaddr), data.size());
  EXPECT_EQ(client.recvfrom(buffer, peer), data.size());
  EXPECT_EQ(buffer, data);
  EXPECT_EQ(peer, server_sockaddr);

  EXPECT_EQ(client.connect(server_sockaddr), skynet::connect_status::kSuccess);
  EXPECT_EQ(client.send(data), data.size());
  EXPECT_EQ(client.recv(buffer), data.size());
  EXPECT_EQ(buffer, data);
}

template <typename Socket, skynet::concepts::sockaddr Sockaddr>
constexpr void nonblocking_handshake(Sockaddr const & sockaddr,
                                     auto const & connect_status) noexcept
  requires(skynet::concepts::stream_socket<Socket, Sockaddr>)
{
  std::vector<std::byte> const data{std::byte(1), std::byte(2), std::byte(3)};

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

  EXPECT_EQ(client.connect(sockaddr), connect_status);
  Socket peer(skynet::uninitialized);
  while (server.accept(peer) != skynet::accept_status::kSuccess) {
  }

  EXPECT_EQ(peer.send(data), data.size());
  std::vector<std::byte> buffer(data.size());
  while (client.recv(buffer) != data.size()) {
  }
  EXPECT_EQ(buffer, data);
}

template <typename Socket, skynet::concepts::sockaddr Sockaddr>
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
constexpr void nonblocking_handshake(Sockaddr const & server_sockaddr,
                                     Sockaddr const & client_sockaddr) noexcept
  requires(skynet::concepts::dgram_socket<Socket, Sockaddr>)
{
  std::vector<std::byte> const data{std::byte(1), std::byte(2), std::byte(3)};

  Socket server;
  server.set_option(skynet::options::nonblock(true));
  server.set_option(skynet::options::reuseaddr(true));
  server.set_option(skynet::options::reuseport(true));
  server.bind(server_sockaddr);

  Socket client;
  client.set_option(skynet::options::nonblock(true));
  client.set_option(skynet::options::reuseaddr(true));
  client.set_option(skynet::options::reuseport(true));
  client.bind(client_sockaddr);

  auto peer(server_sockaddr);
  std::vector<std::byte> buffer(data.size());

  EXPECT_EQ(server.sendto(data, client_sockaddr), data.size());
  while (client.recvfrom(buffer, peer) != data.size()) {
  }
  EXPECT_EQ(buffer, data);
  EXPECT_EQ(peer, server_sockaddr);

  server.connect(client_sockaddr);
  client.connect(server_sockaddr);

  EXPECT_EQ(client.send(data), data.size());
  while (server.recv(buffer) != data.size()) {
  }
  EXPECT_EQ(buffer, data);
}

}  // namespace tests::unit::templates
