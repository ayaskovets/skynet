#include <skynet/inet/port.hpp>

#include <gtest/gtest.h>

namespace tests::unit::inet {

TEST(inet_port, size) {
  static_assert(sizeof(skynet::inet::port) == 2);
  static_assert(alignof(skynet::inet::port) == 2);
}

TEST(inet_port, construction) {
  EXPECT_NE(skynet::inet::port(1234),
            skynet::inet::port(skynet::network_byte_order_t{}, 1234));
  EXPECT_EQ(skynet::inet::port(65535),
            skynet::inet::port(skynet::network_byte_order_t{}, 65535));
  EXPECT_EQ(skynet::inet::port(0),
            skynet::inet::port(skynet::network_byte_order_t{}, 0));
}

}  // namespace tests::unit::inet
