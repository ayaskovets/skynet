#include <skynet/inet/sockaddr.hpp>

#include <gtest/gtest.h>

namespace tests::unit::inet {

TEST(inet_sockaddr, size) {
  static_assert(sizeof(skynet::inet::sockaddr) == 24);
  static_assert(alignof(skynet::inet::sockaddr) == 8);
}

TEST(inet_sockaddr, equality) {
  EXPECT_EQ(skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                                   skynet::inet::port(1)),
            skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                                   skynet::inet::port(1)));
  EXPECT_NE(skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                                   skynet::inet::port(2)),
            skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                                   skynet::inet::port(1)));
  EXPECT_NE(
      skynet::inet::sockaddr(skynet::inet::ip::kAny(), skynet::inet::port(1)),
      skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                             skynet::inet::port(1)));
}

TEST(inet_sockaddr, get_parts) {
  // NOLINTNEXTLINE(readability-identifier-length)
  skynet::inet::ip const ip("100.1.4.224");
  skynet::inet::port const port(1452);
  skynet::inet::sockaddr const sockaddr(ip, port);
  EXPECT_EQ(sockaddr.ip(), ip);
  EXPECT_EQ(sockaddr.port(), port);
}

TEST(inet_sockaddr, to_string) {
  EXPECT_EQ(skynet::inet::sockaddr(skynet::inet::ip::kBroadcast(),
                                   skynet::inet::port(65110))
                .to_string(),
            "255.255.255.255:65110");
  EXPECT_EQ(skynet::inet::sockaddr(skynet::inet::ip::kLoopback(),
                                   skynet::inet::port(65110))
                .to_string(),
            "127.0.0.1:65110");
  EXPECT_EQ(skynet::inet::sockaddr(skynet::inet::ip("100.1.4.224"),
                                   skynet::inet::port(1452))
                .to_string(),
            "100.1.4.224:1452");
}

TEST(inet_sockaddr, format) {
  skynet::inet::sockaddr const sockaddr(skynet::inet::ip::kAny(),
                                        skynet::inet::port(0));
  EXPECT_EQ(std::format("{}", sockaddr), sockaddr.to_string());
}

}  // namespace tests::unit::inet
