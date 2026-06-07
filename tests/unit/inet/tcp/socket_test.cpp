#include "../../templates/socket.hpp"

#include <skynet/inet/sockaddr.hpp>
#include <skynet/inet/tcp/socket.hpp>

#include <gtest/gtest.h>

#include "../../templates/templates.hpp"

namespace tests::unit::inet::tcp {

TEST(inet_tcp_socket, size) {
  static_assert(sizeof(skynet::inet::tcp::socket) == 4);
  static_assert(alignof(skynet::inet::tcp::socket) == 4);
}

TEST_T(inet_tcp_socket, unconnected_send_receive, skynet::inet::tcp::socket);

TEST_T(inet_tcp_socket, bind, skynet::inet::tcp::socket,
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9999)));

TEST_T(inet_tcp_socket, connect, skynet::inet::tcp::socket,
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9998)),
       skynet::connect_status::kRefused);

TEST_T(inet_tcp_socket, sockaddrs, skynet::inet::tcp::socket,
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9997)),
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9996)),
       skynet::connect_status::kPending);

TEST_T(inet_tcp_socket, unconnected_accept, skynet::inet::tcp::socket,
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9999)));

TEST_T(inet_tcp_socket, blocking_handshake, skynet::inet::tcp::socket,
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9995)));

TEST_T(inet_tcp_socket, nonblocking_handshake, skynet::inet::tcp::socket,
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9996)),
       skynet::connect_status::kPending);

}  // namespace tests::unit::inet::tcp
