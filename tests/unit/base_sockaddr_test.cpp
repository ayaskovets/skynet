#include <skynet/base_sockaddr.hpp>

#include <gtest/gtest.h>

namespace tests::unit {

namespace {

class sockaddr final : public skynet::base_sockaddr {
 public:
  explicit sockaddr(skynet::family family) : base_sockaddr(family) {}

 public:
  using base_sockaddr::native_sockaddr;
};

}  // namespace

TEST(net_sockets_base_sockaddr, size) {
  static_assert(sizeof(sockaddr) == 24);
  static_assert(alignof(sockaddr) == 8);
}

class net_sockets_base_sockaddr
    : public ::testing::TestWithParam<skynet::family> {};

TEST_P(net_sockets_base_sockaddr, smoke) {
  auto const & family = GetParam();
  sockaddr const sockaddr(family);

  EXPECT_GT(sockaddr.length(), 0);
  EXPECT_EQ(sockaddr.family(), family);
  EXPECT_TRUE(sockaddr.native_sockaddr());
  EXPECT_NO_THROW(std::ignore = sockaddr.to_string());
}

TEST_P(net_sockets_base_sockaddr, copy) {
  auto const & family = GetParam();
  sockaddr const original(family);
  // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
  sockaddr const copy(original);

  EXPECT_EQ(original.family(), copy.family());
  EXPECT_EQ(original.length(), copy.length());
  EXPECT_NE(original.native_sockaddr(), copy.native_sockaddr());
  EXPECT_EQ(original, copy);
}

TEST_P(net_sockets_base_sockaddr, move) {
  auto const & family = GetParam();
  sockaddr original(family);
  sockaddr const move(std::move(original));

  EXPECT_GT(move.length(), 0);
  EXPECT_EQ(move.family(), family);
  EXPECT_TRUE(move.native_sockaddr());
  EXPECT_NO_THROW(std::ignore = move.to_string());
}

INSTANTIATE_TEST_SUITE_P(net_sockets_base_sockaddr, net_sockets_base_sockaddr,
                         ::testing::Values(skynet::family::kInet,
                                           skynet::family::kInet6,
                                           skynet::family::kUnix));

}  // namespace tests::unit
