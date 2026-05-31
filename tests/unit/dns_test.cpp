#include <skynet/dns.hpp>

#include <gtest/gtest.h>

namespace tests::unit {

TEST(dns, resolve_localhost_udp_ip4) {
  auto const results = skynet::resolve("localhost", skynet::family::kInet,
                                       skynet::socktype::kDgram);
  EXPECT_EQ(results.size(), 1);

  auto const & sockaddr = results.at(0);
  EXPECT_EQ(std::get<skynet::inet::sockaddr>(sockaddr).ip(),
            skynet::inet::ip("127.0.0.1"));
}

TEST(dns, resolve_localhost_tcp_ip6) {
  auto const results = skynet::resolve("localhost", skynet::family::kInet6,
                                       skynet::socktype::kStream);
  EXPECT_EQ(results.size(), 1);

  auto const & sockaddr = results.at(0);
  EXPECT_EQ(std::get<skynet::inet6::sockaddr>(sockaddr).ip(),
            skynet::inet6::ip("::1"));
}

TEST(dns, resolve_localhost_unspec) {
  auto const results = skynet::resolve("localhost", skynet::family::kUnspec,
                                       skynet::socktype::kUnspec);
  EXPECT_EQ(results.size(), 4);
}

}  // namespace tests::unit
