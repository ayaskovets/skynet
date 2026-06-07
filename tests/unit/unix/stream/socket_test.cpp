#include "../../templates/socket.hpp"

#include <skynet/unix/stream/socket.hpp>

#include <gtest/gtest.h>

#include "../../templates/templates.hpp"

namespace tests::unit::unix::stream {

TEST(unix_stream_socket, size) {
  static_assert(sizeof(skynet::unix::stream::socket) == 4);
  static_assert(alignof(skynet::unix::stream::socket) == 4);
}

TEST_T(unix_stream_socket, unconnected_send_receive,
       skynet::unix::stream::socket);

TEST_T(unix_stream_socket, bind, skynet::unix::stream::socket,
       skynet::unix::sockaddr("net_unix_stream_socket_bind"));

TEST_T(unix_stream_socket, connect, skynet::unix::stream::socket,
       skynet::unix::sockaddr("net_unix_stream_socket_connect"),
       skynet::connect_status::kRefused);

TEST_T(unix_stream_socket, sockaddrs, skynet::unix::stream::socket,
       skynet::unix::sockaddr("net_unix_stream_socket_bind_sockaddr"),
       skynet::unix::sockaddr("net_unix_stream_socket_connect_sockaddr"),
       skynet::connect_status::kRefused);

TEST_T(unix_stream_socket, unconnected_accept, skynet::unix::stream::socket,
       skynet::unix::sockaddr("net_unix_stream_socket_unconnected_accept"));

TEST_T(unix_stream_socket, blocking_handshake, skynet::unix::stream::socket,
       skynet::unix::sockaddr("net_unix_stream_socket_blocking_handshake"));

TEST_T(unix_stream_socket, nonblocking_handshake, skynet::unix::stream::socket,
       skynet::unix::sockaddr("net_unix_stream_socket_nonblocking_handshake"),
       skynet::connect_status::kSuccess);

TEST(unix_stream_socket, unlink) {
  skynet::unix::sockaddr const sockaddr(
      skynet::unix::sockaddr("net_unix_stream_socket_unlink"));

  skynet::unix::stream::socket socket;
  EXPECT_EQ(socket.unlink_bind(sockaddr), skynet::bind_status::kSuccess);

  skynet::unix::stream::socket other;
  EXPECT_EQ(other.bind(sockaddr), skynet::bind_status::kInUse);

  socket.unlink_close();
  EXPECT_EQ(other.bind(sockaddr), skynet::bind_status::kSuccess);
}

}  // namespace tests::unit::unix::stream
