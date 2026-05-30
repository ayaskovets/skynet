#include <skynet/inet6/ip.hpp>

#include <gtest/gtest.h>

namespace tests::unit::inet6 {

TEST(inet6_ip, size) {
  static_assert(sizeof(skynet::inet6::ip) == 16);
  static_assert(alignof(skynet::inet6::ip) == 1);
}

TEST(inet6_ip, loopback) {
  EXPECT_EQ(skynet::inet6::ip("::1"), skynet::inet6::ip::kLoopback());
}

TEST(inet6_ip, any) {
  EXPECT_EQ(skynet::inet6::ip("::"), skynet::inet6::ip::kAny());
}

TEST(inet6_ip, construction) {
  std::array<std::byte, 16> const bytes{
      std::byte(0), std::byte(0), std::byte(0x25), std::byte(0x5e),
      std::byte(0), std::byte(0), std::byte(0),    std::byte(0),
      std::byte(0), std::byte(0), std::byte(0),    std::byte(0),
      std::byte(0), std::byte(0), std::byte(0),    std::byte(0)};
  std::string const string("0:255e::");

  skynet::inet6::ip const from_bytes(skynet::network_byte_order_t{},
                                     std::span<std::byte const, 16>(bytes));
  skynet::inet6::ip const from_string(string);

  EXPECT_EQ(from_bytes, from_string);
  EXPECT_EQ(bytes, from_bytes.to_bytes(skynet::network_byte_order_t{}));
  EXPECT_EQ(from_bytes.to_string(), string);
  EXPECT_EQ(from_bytes.to_bytes(skynet::network_byte_order_t{}),
            from_string.to_bytes(skynet::network_byte_order_t{}));
  EXPECT_EQ(from_bytes.to_string(), from_string.to_string());
}

TEST(inet6_ip, invalid) {
  EXPECT_ANY_THROW(skynet::inet6::ip("2002::x"));
  EXPECT_ANY_THROW(skynet::inet6::ip("256.0.0.0"));
  EXPECT_ANY_THROW(skynet::inet6::ip("11132123123abc"));
}

TEST(inet6_ip, to_string) {
  EXPECT_EQ(skynet::inet6::ip("::f").to_string(), "::f");
  EXPECT_EQ(skynet::inet6::ip("2002:cb00:7101::").to_string(),
            "2002:cb00:7101::");
  EXPECT_EQ(skynet::inet6::ip("2001:db8::1").to_string(), "2001:db8::1");
}

TEST(inet6_ip, format) {
  // NOLINTNEXTLINE(readability-identifier-length)
  skynet::inet6::ip const ip(skynet::inet6::ip::kAny());
  EXPECT_EQ(std::format("{}", ip), ip.to_string());
}

}  // namespace tests::unit::inet6
