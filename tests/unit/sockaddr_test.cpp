#include <skynet/base_sockaddr.hpp>
#include <skynet/sockaddr.hpp>

#include <gtest/gtest.h>

namespace tests::unit {

TEST(queue, base_sockaddr) {
  using sockaddr_t = skynet::base_sockaddr;

  static_assert(skynet::concepts::sockaddr<sockaddr_t>);
}

}  // namespace tests::unit
