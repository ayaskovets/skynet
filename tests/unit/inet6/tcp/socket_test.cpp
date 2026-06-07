#include "../../templates/socket.hpp"

#include <skynet/inet6/sockaddr.hpp>
#include <skynet/inet6/tcp/socket.hpp>

#include <gtest/gtest.h>

#include "../../templates/templates.hpp"

namespace tests::unit::inet6::tcp {

TEST(inet6_tcp_socket, size) {
  static_assert(sizeof(skynet::inet6::tcp::socket) == 4);
  static_assert(alignof(skynet::inet6::tcp::socket) == 4);
}

TEST_T(inet6_tcp_socket, unconnected_send_receive, skynet::inet6::tcp::socket);

TEST_T(inet6_tcp_socket, bind, skynet::inet6::tcp::socket,
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9999)));

TEST_T(inet6_tcp_socket, connect, skynet::inet6::tcp::socket,
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9998)),
       skynet::connect_status::kRefused);

TEST_T(inet6_tcp_socket, sockaddrs, skynet::inet6::tcp::socket,
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9997)),
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9996)),
       skynet::connect_status::kPending);

TEST_T(inet6_tcp_socket, unconnected_accept, skynet::inet6::tcp::socket,
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9999)));

TEST_T(inet6_tcp_socket, blocking_handshake, skynet::inet6::tcp::socket,
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9995)));

TEST_T(inet6_tcp_socket, nonblocking_handshake, skynet::inet6::tcp::socket,
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9996)),
       skynet::connect_status::kPending);

}  // namespace tests::unit::inet6::tcp
