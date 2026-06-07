
#include "../../templates/socket.hpp"

#include <skynet/inet6/sockaddr.hpp>
#include <skynet/inet6/udp/socket.hpp>

#include <gtest/gtest.h>

#include "../../templates/templates.hpp"

namespace tests::unit::inet6::udp {

TEST(inet6_udp_socket, size) {
  static_assert(sizeof(skynet::inet6::udp::socket) == 4);
  static_assert(alignof(skynet::inet6::udp::socket) == 4);
}

TEST_T(inet6_udp_socket, unconnected_send_receive, skynet::inet6::udp::socket,
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9999)));

TEST_T(inet6_udp_socket, bind, skynet::inet6::udp::socket,
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9999)));

TEST_T(inet6_udp_socket, connect, skynet::inet6::udp::socket,
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9998)),
       skynet::connect_status::kSuccess);

TEST_T(inet6_udp_socket, sockaddrs, skynet::inet6::udp::socket,
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9997)),
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9996)),
       skynet::connect_status::kSuccess);

TEST_T(inet6_udp_socket, blocking_handshake, skynet::inet6::udp::socket,
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9995)),
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9996)));

TEST_T(inet6_udp_socket, nonblocking_handshake, skynet::inet6::udp::socket,
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9995)),
       skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                               skynet::inet6::port(9996)));

}  // namespace tests::unit::inet6::udp
