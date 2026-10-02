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
  using socket_type   = skynet::base_socket;
  using sockaddr_type = skynet::base_sockaddr;

  static_assert(!skynet::concepts::socket<socket_type, sockaddr_type>);
  static_assert(!skynet::concepts::stream_socket<socket_type, sockaddr_type>);
  static_assert(!skynet::concepts::dgram_socket<socket_type, sockaddr_type>);
}

TEST(socket, inet_tcp_socket) {
  using socket_type   = skynet::inet::tcp::socket;
  using sockaddr_type = skynet::inet::sockaddr;

  static_assert(skynet::concepts::socket<socket_type, sockaddr_type>);
  static_assert(skynet::concepts::stream_socket<socket_type, sockaddr_type>);
  static_assert(!skynet::concepts::dgram_socket<socket_type, sockaddr_type>);
}

TEST(socket, inet_udp_socket) {
  using socket_type   = skynet::inet::udp::socket;
  using sockaddr_type = skynet::inet::sockaddr;

  static_assert(skynet::concepts::socket<socket_type, sockaddr_type>);
  static_assert(!skynet::concepts::stream_socket<socket_type, sockaddr_type>);
  static_assert(skynet::concepts::dgram_socket<socket_type, sockaddr_type>);
}

TEST(socket, inet6_tcp_socket) {
  using socket_type   = skynet::inet6::tcp::socket;
  using sockaddr_type = skynet::inet6::sockaddr;

  static_assert(skynet::concepts::socket<socket_type, sockaddr_type>);
  static_assert(skynet::concepts::stream_socket<socket_type, sockaddr_type>);
  static_assert(!skynet::concepts::dgram_socket<socket_type, sockaddr_type>);
}

TEST(socket, inet6_udp_socket) {
  using socket_type   = skynet::inet6::udp::socket;
  using sockaddr_type = skynet::inet6::sockaddr;

  static_assert(skynet::concepts::socket<socket_type, sockaddr_type>);
  static_assert(!skynet::concepts::stream_socket<socket_type, sockaddr_type>);
  static_assert(skynet::concepts::dgram_socket<socket_type, sockaddr_type>);
}

TEST(socket, unix_dgram_socket) {
  using socket_type   = skynet::unix::dgram::socket;
  using sockaddr_type = skynet::unix::sockaddr;

  static_assert(skynet::concepts::socket<socket_type, sockaddr_type>);
  static_assert(!skynet::concepts::stream_socket<socket_type, sockaddr_type>);
  static_assert(skynet::concepts::dgram_socket<socket_type, sockaddr_type>);
}

TEST(socket, unix_stream_socket) {
  using socket_type   = skynet::unix::stream::socket;
  using sockaddr_type = skynet::unix::sockaddr;

  static_assert(skynet::concepts::socket<socket_type, sockaddr_type>);
  static_assert(skynet::concepts::stream_socket<socket_type, sockaddr_type>);
  static_assert(!skynet::concepts::dgram_socket<socket_type, sockaddr_type>);
}

}  // namespace tests::unit
