#include <skynet/base_sockaddr.hpp>
#include <skynet/inet/sockaddr.hpp>
#include <skynet/inet6/sockaddr.hpp>
#include <skynet/sockaddr.hpp>
#include <skynet/unix/sockaddr.hpp>

#include <gtest/gtest.h>

namespace tests::unit {

TEST(sockaddr, base_sockaddr) {
  static_assert(skynet::concepts::sockaddr<skynet::base_sockaddr>);
}

TEST(sockaddr, inet_sockaddr) {
  static_assert(skynet::concepts::sockaddr<skynet::inet::sockaddr>);
}

TEST(sockaddr, inet6_sockaddr) {
  static_assert(skynet::concepts::sockaddr<skynet::inet6::sockaddr>);
}

TEST(sockaddr, unix_sockaddr) {
  static_assert(skynet::concepts::sockaddr<skynet::unix::sockaddr>);
}

}  // namespace tests::unit
