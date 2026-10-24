#include <skynet/inet/tcp/socket.hpp>
#include <skynet/multiplexing.hpp>

#include <gtest/gtest.h>

namespace tests::unit {

TEST(multiplexing, size) {
  static_assert(sizeof(skynet::poller) == 32);
  static_assert(alignof(skynet::poller) == 8);
}

TEST(multiplexing, move) {
  skynet::poller moved_from(1);
  skynet::poller const moved_to(std::move(moved_from));
}

TEST(multiplexing, insert_erase) {
  skynet::poller poller(1);
  skynet::inet::tcp::socket socket;

  poller.insert_or_assign(socket, skynet::poller::kPollIn, nullptr);

  poller.erase(socket);

  EXPECT_EQ(socket.family(), skynet::family::kInet);
  EXPECT_EQ(socket.socktype(), skynet::socktype::kStream);
}

TEST(multiplexing, insert_or_assign) {
  skynet::poller poller(1);
  skynet::inet::tcp::socket socket;

  poller.insert_or_assign(socket, skynet::poller::kPollIn, nullptr);

  EXPECT_NO_THROW(
      poller.insert_or_assign(socket, skynet::poller::kPollOut, nullptr));

  poller.erase(socket);
}

TEST(multiplexing, socket_reuse) {
  skynet::poller poller(1);
  skynet::inet::tcp::socket first;
  skynet::inet::tcp::socket second;

  poller.insert_or_assign(first, skynet::poller::kPollIn, nullptr);
  poller.insert_or_assign(second, skynet::poller::kPollOut, nullptr);

  poller.erase(first);
  EXPECT_NO_THROW(
      poller.insert_or_assign(second, skynet::poller::kPollIn, nullptr));

  poller.insert_or_assign(first, skynet::poller::kPollIn, nullptr);

  poller.erase(second);
  poller.erase(first);
}

}  // namespace tests::unit
