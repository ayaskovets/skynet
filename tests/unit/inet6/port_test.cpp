#include <skynet/inet6/port.hpp>

#include <gtest/gtest.h>

namespace tests::unit::inet6 {

TEST(inet6_port, size) {
  static_assert(sizeof(skynet::inet6::port) == 2);
  static_assert(alignof(skynet::inet6::port) == 2);
}

TEST(inet6_port, construction) {
  EXPECT_NE(skynet::inet6::port(1234),
            skynet::inet6::port(skynet::network_byte_order_t{}, 1234));
  EXPECT_EQ(skynet::inet6::port(65535),
            skynet::inet6::port(skynet::network_byte_order_t{}, 65535));
  EXPECT_EQ(skynet::inet6::port(0),
            skynet::inet6::port(skynet::network_byte_order_t{}, 0));
}

}  // namespace tests::unit::inet6
