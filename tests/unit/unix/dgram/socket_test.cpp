#include "../../templates/socket.hpp"

#include <skynet/unix/dgram/socket.hpp>

#include <gtest/gtest.h>

#include "../../templates/templates.hpp"

namespace tests::unit::unix::dgram {

TEST(unix_dgram_socket, size) {
  static_assert(sizeof(skynet::unix::dgram::socket) == 4);
  static_assert(alignof(skynet::unix::dgram::socket) == 4);
}

TEST(unix_dgram_socket, unconnected_send_receive) {
  std::vector<std::byte> const data{std::byte(1), std::byte(2), std::byte(3)};

  skynet::unix::dgram::socket socket;
  socket.set_option(skynet::nonblock(true));

  EXPECT_ANY_THROW(socket.send(data));
  std::vector<std::byte> buffer(data.size());
  EXPECT_EQ(socket.recv(buffer), 0);
}

TEST_T(unix_dgram_socket, bind, skynet::unix::dgram::socket,
       skynet::unix::sockaddr("net_unix_dgram_socket_bind"));

TEST_T(unix_dgram_socket, connect, skynet::unix::dgram::socket,
       skynet::unix::sockaddr("net_unix_dgram_socket_connect"),
       skynet::connect_status::kRefused);

TEST_T(unix_dgram_socket, sockaddrs, skynet::unix::dgram::socket,
       skynet::unix::sockaddr("net_unix_dgram_socket_bind_sockaddr"),
       skynet::unix::sockaddr("net_unix_dgram_socket_connect_sockaddr"),
       skynet::connect_status::kRefused);

TEST_T(
    unix_dgram_socket, blocking_handshake, skynet::unix::dgram::socket,
    skynet::unix::sockaddr("net_unix_dgram_socket_blocking_handshake_server"),
    skynet::unix::sockaddr("net_unix_dgram_socket_blocking_handshake_client"));

TEST_T(unix_dgram_socket, nonblocking_handshake, skynet::unix::dgram::socket,
       skynet::unix::sockaddr(
           "net_unix_dgram_socket_nonblocking_handshake_server"),
       skynet::unix::sockaddr(
           "net_unix_dgram_socket_nonblocking_handshake_client"));

TEST(unix_dgram_socket, unlink) {
  skynet::unix::sockaddr const sockaddr(
      skynet::unix::sockaddr("net_unix_dgram_socket_unlink"));

  skynet::unix::dgram::socket socket;
  EXPECT_EQ(socket.unlink_bind(sockaddr), skynet::bind_status::kSuccess);

  skynet::unix::dgram::socket other;
  EXPECT_EQ(other.bind(sockaddr), skynet::bind_status::kInUse);

  socket.unlink_close();
  EXPECT_EQ(other.bind(sockaddr), skynet::bind_status::kSuccess);
}

}  // namespace tests::unit::unix::dgram
