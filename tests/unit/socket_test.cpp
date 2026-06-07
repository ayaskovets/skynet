#include <skynet/base_sockaddr.hpp>
#include <skynet/base_socket.hpp>
#include <skynet/inet/sockaddr.hpp>
#include <skynet/inet/tcp/socket.hpp>
#include <skynet/inet/udp/socket.hpp>
#include <skynet/inet6/sockaddr.hpp>
#include <skynet/inet6/tcp/socket.hpp>
#include <skynet/inet6/udp/socket.hpp>
#include <skynet/socket.hpp>
#include <skynet/unix/dgram/socket.hpp>
#include <skynet/unix/sockaddr.hpp>
#include <skynet/unix/stream/socket.hpp>

#include <gtest/gtest.h>

namespace tests::unit {

TEST(socket, base_socket) {
  using socket_t   = skynet::base_socket;
  using sockaddr_t = skynet::base_sockaddr;

  static_assert(!skynet::concepts::socket<socket_t, sockaddr_t>);
  static_assert(!skynet::concepts::stream_socket<socket_t, sockaddr_t>);
  static_assert(!skynet::concepts::dgram_socket<socket_t, sockaddr_t>);
}

TEST(socket, inet_tcp_socket) {
  using socket_t   = skynet::inet::tcp::socket;
  using sockaddr_t = skynet::inet::sockaddr;

  static_assert(skynet::concepts::socket<socket_t, sockaddr_t>);
  static_assert(skynet::concepts::stream_socket<socket_t, sockaddr_t>);
  static_assert(!skynet::concepts::dgram_socket<socket_t, sockaddr_t>);
}

TEST(socket, inet_udp_socket) {
  using socket_t   = skynet::inet::udp::socket;
  using sockaddr_t = skynet::inet::sockaddr;

  static_assert(skynet::concepts::socket<socket_t, sockaddr_t>);
  static_assert(!skynet::concepts::stream_socket<socket_t, sockaddr_t>);
  static_assert(skynet::concepts::dgram_socket<socket_t, sockaddr_t>);
}

TEST(socket, inet6_tcp_socket) {
  using socket_t   = skynet::inet6::tcp::socket;
  using sockaddr_t = skynet::inet6::sockaddr;

  static_assert(skynet::concepts::socket<socket_t, sockaddr_t>);
  static_assert(skynet::concepts::stream_socket<socket_t, sockaddr_t>);
  static_assert(!skynet::concepts::dgram_socket<socket_t, sockaddr_t>);
}

TEST(socket, inet6_udp_socket) {
  using socket_t   = skynet::inet6::udp::socket;
  using sockaddr_t = skynet::inet6::sockaddr;

  static_assert(skynet::concepts::socket<socket_t, sockaddr_t>);
  static_assert(!skynet::concepts::stream_socket<socket_t, sockaddr_t>);
  static_assert(skynet::concepts::dgram_socket<socket_t, sockaddr_t>);
}

TEST(socket, unix_dgram_socket) {
  using socket_t   = skynet::unix::dgram::socket;
  using sockaddr_t = skynet::unix::sockaddr;

  static_assert(skynet::concepts::socket<socket_t, sockaddr_t>);
  static_assert(!skynet::concepts::stream_socket<socket_t, sockaddr_t>);
  static_assert(skynet::concepts::dgram_socket<socket_t, sockaddr_t>);
}

TEST(socket, unix_stream_socket) {
  using socket_t   = skynet::unix::stream::socket;
  using sockaddr_t = skynet::unix::sockaddr;

  static_assert(skynet::concepts::socket<socket_t, sockaddr_t>);
  static_assert(skynet::concepts::stream_socket<socket_t, sockaddr_t>);
  static_assert(!skynet::concepts::dgram_socket<socket_t, sockaddr_t>);
}

}  // namespace tests::unit
