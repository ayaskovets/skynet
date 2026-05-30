#include <skynet/inet6/sockaddr.hpp>

#include <gtest/gtest.h>

namespace tests::unit::inet6 {

TEST(inet6_sockaddr, size) {
  static_assert(sizeof(skynet::inet6::sockaddr) == 24);
  static_assert(alignof(skynet::inet6::sockaddr) == 8);
}

TEST(inet6_sockaddr, equality) {
  EXPECT_EQ(skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                                    skynet::inet6::port(1)),
            skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                                    skynet::inet6::port(1)));
  EXPECT_NE(skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                                    skynet::inet6::port(2)),
            skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                                    skynet::inet6::port(1)));
  EXPECT_NE(skynet::inet6::sockaddr(skynet::inet6::ip::kAny(),
                                    skynet::inet6::port(1)),
            skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                                    skynet::inet6::port(1)));
}

TEST(inet6_sockaddr, get_parts) {
  // NOLINTNEXTLINE(readability-identifier-length)
  skynet::inet6::ip const ip("1e:fe56::");
  skynet::inet6::port const port(1452);
  skynet::inet6::sockaddr const sockaddr(ip, port);
  EXPECT_EQ(sockaddr.ip(), ip);
  EXPECT_EQ(sockaddr.port(), port);
}

TEST(inet6_sockaddr, to_string) {
  EXPECT_EQ(skynet::inet6::sockaddr(skynet::inet6::ip::kLoopback(),
                                    skynet::inet6::port(65110))
                .to_string(),
            "[::1]:65110");
  EXPECT_EQ(skynet::inet6::sockaddr(skynet::inet6::ip("2001:db8::1"),
                                    skynet::inet6::port(1123))
                .to_string(),
            "[2001:db8::1]:1123");
}

TEST(inet6_sockaddr, format) {
  skynet::inet6::sockaddr const sockaddr(skynet::inet6::ip::kAny(),
                                         skynet::inet6::port(0));
  EXPECT_EQ(std::format("{}", sockaddr), sockaddr.to_string());
}

}  // namespace tests::unit::inet6
