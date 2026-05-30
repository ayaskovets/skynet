#include <skynet/inet/ip.hpp>

#include <gtest/gtest.h>

namespace tests::unit::inet {

TEST(inet_ip, size) {
  static_assert(sizeof(skynet::inet::ip) == 4);
  static_assert(alignof(skynet::inet::ip) == 4);
}

TEST(inet_ip, loopback) {
  EXPECT_EQ(skynet::inet::ip("127.0.0.1"), skynet::inet::ip::kLoopback());
}

TEST(inet_ip, any) {
  EXPECT_EQ(skynet::inet::ip("0.0.0.0"), skynet::inet::ip::kAny());
}

TEST(inet_ip, broadcast) {
  EXPECT_EQ(skynet::inet::ip("255.255.255.255"),
            skynet::inet::ip::kBroadcast());
}

TEST(inet_ip, construction) {
  std::uint32_t const bytes{0xFFFFFF00};
  std::string const string("255.255.255.0");

  skynet::inet::ip const from_bytes(bytes);
  skynet::inet::ip const from_string(string);

  EXPECT_EQ(from_bytes, from_string);
  EXPECT_EQ(bytes, from_bytes.to_bytes());
  EXPECT_EQ(from_bytes.to_string(), string);
  EXPECT_EQ(from_bytes.to_bytes(), from_string.to_bytes());
  EXPECT_EQ(from_bytes.to_string(), from_string.to_string());
}

TEST(inet_ip, network_order) {
  std::uint32_t const bytes{0xFFFFFF00};

  skynet::inet::ip const host_byte_order(bytes);
  skynet::inet::ip const network_byte_order(skynet::network_byte_order_t{},
                                            bytes);

  EXPECT_EQ(host_byte_order.to_bytes(), bytes);
  EXPECT_NE(host_byte_order.to_bytes(skynet::network_byte_order_t{}), bytes);
  EXPECT_NE(network_byte_order.to_bytes(), bytes);
  EXPECT_EQ(network_byte_order.to_bytes(skynet::network_byte_order_t{}), bytes);
}

TEST(inet_ip, invalid) {
  EXPECT_ANY_THROW(skynet::inet::ip(""));
  EXPECT_ANY_THROW(skynet::inet::ip("256.0.0.0"));
  EXPECT_ANY_THROW(skynet::inet::ip("11132123123abc"));
}

TEST(inet_ip, to_string) {
  EXPECT_EQ(skynet::inet::ip("255.255.255.0").to_string(), "255.255.255.0");
  EXPECT_EQ(skynet::inet::ip("255.255.255.0").to_string(), "255.255.255.0");
}

TEST(inet_ip, format) {
  // NOLINTNEXTLINE(readability-identifier-length)
  skynet::inet::ip const ip(skynet::inet::ip::kAny());
  EXPECT_EQ(std::format("{}", ip), ip.to_string());
}

}  // namespace tests::unit::inet
