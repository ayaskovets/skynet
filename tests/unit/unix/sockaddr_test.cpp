#include <skynet/unix/sockaddr.hpp>

#include <gtest/gtest.h>

namespace tests::unit::unix {

TEST(unix_sockaddr, size) {
  static_assert(sizeof(skynet::unix::sockaddr) == 24);
  static_assert(alignof(skynet::unix::sockaddr) == 8);
}

TEST(unix_sockaddr, construction) {
  EXPECT_NO_THROW(skynet::unix::sockaddr(""));
  EXPECT_NO_THROW(skynet::unix::sockaddr("/path/to/a/file"));
  EXPECT_ANY_THROW(skynet::unix::sockaddr(
      "/loooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo"
      "oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo"
      "oooooooooooooooooooooooooong"));
}

TEST(unix_sockaddr, equality) {
  EXPECT_EQ(skynet::unix::sockaddr::kEmpty(), skynet::unix::sockaddr::kEmpty());
  EXPECT_EQ(skynet::unix::sockaddr("/path/to/a/file"),
            skynet::unix::sockaddr("/path/to/a/file"));
  EXPECT_NE(skynet::unix::sockaddr("/path/to/a/file"),
            skynet::unix::sockaddr("/path/to/a/file2"));
  EXPECT_NE(skynet::unix::sockaddr("/path/to/a/file2"),
            skynet::unix::sockaddr("/path/to/a/file"));
}

TEST(unix_sockaddr, to_string) {
  EXPECT_EQ(skynet::unix::sockaddr("net_unix_sockaddr_to_string").to_string(),
            "net_unix_sockaddr_to_string");
}

TEST(unix_sockaddr, format) {
  skynet::unix::sockaddr const sockaddr("net_unix_sockaddr_to_string");
  EXPECT_EQ(std::format("{}", sockaddr), sockaddr.to_string());
}

}  // namespace tests::unit::unix
