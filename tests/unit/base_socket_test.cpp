#include <skynet/base_socket.hpp>
#include <skynet/options.hpp>

#include <gtest/gtest.h>

namespace tests::unit {

namespace {

class socket final : public skynet::base_socket {
 public:
  explicit socket(skynet::uninitialized_t)
      : skynet::base_socket(skynet::uninitialized) {}
  socket(skynet::family family, skynet::socktype type,
         skynet::protocol protocol)
      : skynet::base_socket(family, type, protocol) {}

 public:
  using base_socket::get_option;
  using base_socket::set_option;
};

}  // namespace

TEST(base_socket, size) {
  static_assert(sizeof(socket) == 4);
  static_assert(alignof(socket) == 4);
}

TEST(base_socket, manual_close) {
  socket sock(skynet::family::kInet, skynet::socktype::kStream,
              skynet::protocol::kTcp);
  EXPECT_NO_THROW(sock.close());
  EXPECT_ANY_THROW(sock.close());
  EXPECT_ANY_THROW(sock.close());
}

TEST(base_socket, self_copy_assignment) {
  socket sock(skynet::family::kInet, skynet::socktype::kStream,
              skynet::protocol::kTcp);

  socket & ref = sock;
  sock         = ref;
  EXPECT_EQ(sock, ref);
  EXPECT_NO_THROW(sock.close());
  EXPECT_ANY_THROW(sock.close());
}

TEST(base_socket, self_move_assignment) {
  socket sock(skynet::family::kInet, skynet::socktype::kStream,
              skynet::protocol::kTcp);
  socket & ref = sock;
  sock         = std::move(ref);
  EXPECT_EQ(sock, ref);
  EXPECT_NO_THROW(sock.close());
  EXPECT_ANY_THROW(sock.close());
}

TEST(base_socket, transfer_ownership) {
  socket moved_from(skynet::family::kInet, skynet::socktype::kStream,
                    skynet::protocol::kTcp);
  socket moved_to = std::move(moved_from);
  EXPECT_NE(moved_from, moved_to);
  EXPECT_ANY_THROW(moved_from.close());
  EXPECT_NE(moved_from, moved_to);
  EXPECT_NO_THROW(moved_to.close());
  EXPECT_EQ(moved_from, moved_to);
}

TEST(base_socket, duplicate) {
  socket original(skynet::family::kInet, skynet::socktype::kStream,
                  skynet::protocol::kTcp);
  socket copy(original);
  EXPECT_NE(original, copy);
  EXPECT_NO_THROW(original.close());
  EXPECT_NO_THROW(copy.close());
  EXPECT_EQ(original, copy);

  copy = socket(skynet::family::kInet, skynet::socktype::kStream,
                skynet::protocol::kTcp);
  EXPECT_NO_THROW(copy.close());
}

TEST(base_socket, equality) {
  socket const sock(skynet::family::kInet, skynet::socktype::kStream,
                    skynet::protocol::kTcp);
  EXPECT_EQ(sock, sock);
}

class base_socket
    : public ::testing::TestWithParam<
          std::tuple<skynet::family, skynet::socktype, skynet::protocol>> {};

TEST_P(base_socket, options) try {
  auto const & [family, type, protocol] = GetParam();
  socket socket(family, type, protocol);

  EXPECT_EQ(socket.family(), family);
  EXPECT_EQ(socket.socktype(), type);

  {
    skynet::broadcast option{};
    socket.get_option(option);
    EXPECT_EQ(*option, false);
    *option = true;
    socket.set_option(option);
    socket.get_option(option);
    EXPECT_EQ(*option, true);
  }

  {
    skynet::keepalive option{};
    socket.get_option(option);
    EXPECT_EQ(*option, false);
    *option = true;
    socket.set_option(option);
    socket.get_option(option);
    EXPECT_EQ(*option, true);
  }

  {
    skynet::linger option{std::chrono::seconds(0)};
    socket.get_option(option);
    EXPECT_NE(*option, std::chrono::seconds(0));
    *option = std::chrono::seconds(5);
    socket.set_option(option);
    socket.get_option(option);
    EXPECT_EQ(*option, std::chrono::seconds(5));
  }

  {
    skynet::nonblock option{};
    socket.get_option(option);
    EXPECT_EQ(*option, false);
    *option = true;
    socket.set_option(option);
    socket.get_option(option);
    EXPECT_EQ(*option, true);
  }

  {
    skynet::oobinline option{};
    socket.get_option(option);
    EXPECT_EQ(*option, false);
    *option = true;
    socket.set_option(option);
    socket.get_option(option);
    EXPECT_EQ(*option, true);
  }

  {
    skynet::rcvbuf option{0};
    socket.get_option(option);
    EXPECT_NE(*option, 0);
    *option = 1024;
    socket.set_option(option);
    socket.get_option(option);
    EXPECT_EQ(*option, 1024);
  }

  {
    skynet::reuseaddr option{};
    socket.get_option(option);
    EXPECT_EQ(*option, false);
    *option = true;
    socket.set_option(option);
    socket.get_option(option);
    EXPECT_EQ(*option, true);
  }

  {
    skynet::reuseport option{};
    socket.get_option(option);
    EXPECT_EQ(*option, false);
    *option = true;
    socket.set_option(option);
    socket.get_option(option);
    EXPECT_EQ(*option, true);
  }

  {
    skynet::sndbuf option{0};
    socket.get_option(option);
    EXPECT_NE(*option, 0);
    *option = 1024;
    socket.set_option(option);
    socket.get_option(option);
    EXPECT_EQ(*option, 1024);
  }

  {
    skynet::nodelay option{};
    socket.get_option(option);
    EXPECT_EQ(*option, false);
    *option = true;
    socket.set_option(option);
    socket.get_option(option);
    EXPECT_EQ(*option, true);
  }

} catch (...) {
  GTEST_SKIP() << "unsupported socket instance";
  return;
}

TEST_P(base_socket, copy) {
  auto const & [family, type, protocol] = GetParam();
  try {
    socket(family, type, protocol);
  } catch (...) {
    GTEST_SKIP() << "unsupported socket instance";
  }

  socket original(family, type, protocol);
  socket copy(original);
  EXPECT_EQ(original, original);
  EXPECT_EQ(copy, copy);
  EXPECT_NE(original, copy);
  EXPECT_NO_THROW(original.close());
  EXPECT_NO_THROW(copy.close());
}

TEST_P(base_socket, move) {
  auto const & [family, type, protocol] = GetParam();
  try {
    socket(family, type, protocol);
  } catch (...) {
    GTEST_SKIP() << "unsupported socket instance";
  }

  socket original(family, type, protocol);
  socket move(std::move(original));
  EXPECT_NO_THROW(move.close());
}

INSTANTIATE_TEST_SUITE_P(
    base_socket, base_socket,
    ::testing::Combine(::testing::Values<skynet::family>(skynet::family::kInet,
                                                         skynet::family::kInet6,
                                                         skynet::family::kUnix),
                       ::testing::Values<skynet::socktype>(
                           skynet::socktype::kStream, skynet::socktype::kDgram,
                           skynet::socktype::kRaw, skynet::socktype::kRdm,
                           skynet::socktype::kSeqpacket),
                       ::testing::Values<skynet::protocol>(
                           skynet::protocol::kDefault, skynet::protocol::kTcp,
                           skynet::protocol::kUdp)));

}  // namespace tests::unit
