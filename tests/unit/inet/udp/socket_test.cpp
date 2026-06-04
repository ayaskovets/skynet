
#include "../../templates/socket.hpp"

#include <skynet/inet/sockaddr.hpp>
#include <skynet/inet/udp/socket.hpp>

#include <gtest/gtest.h>

#include "../../templates/templates.hpp"

namespace tests::unit::inet::udp {

TEST(inet_udp_socket, size) {
  static_assert(sizeof(skynet::inet::udp::socket) == 4);
  static_assert(alignof(skynet::inet::udp::socket) == 4);
}

TEST_T(inet_udp_socket, dgram_unconnected_send_receive,
       skynet::inet::udp::socket,
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9999)));

TEST_T(inet_udp_socket, bind, skynet::inet::udp::socket,
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9999)));

TEST_T(inet_udp_socket, connect, skynet::inet::udp::socket,
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9998)),
       skynet::connect_status::kSuccess);

TEST_T(inet_udp_socket, sockaddrs, skynet::inet::udp::socket,
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9997)),
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9996)),
       skynet::connect_status::kSuccess);

TEST_T(inet_udp_socket, dgram_blocking_handshake, skynet::inet::udp::socket,
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9995)),
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9996)));

TEST_T(inet_udp_socket, dgram_nonblocking_handshake, skynet::inet::udp::socket,
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9995)),
       skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                              skynet::inet::port(9996)));

}  // namespace tests::unit::inet::udp
