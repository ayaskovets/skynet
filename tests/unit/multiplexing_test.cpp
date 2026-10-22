#include <skynet/inet/tcp/socket.hpp>
#include <skynet/multiplexing.hpp>

#include <gtest/gtest.h>

namespace tests::unit {

TEST(multiplexing, size) {
  static_assert(sizeof(skynet::poller) == 48);
  static_assert(alignof(skynet::poller) == 8);
}

TEST(multiplexing, move) {
  skynet::poller moved_from;
  skynet::poller const moved_to(std::move(moved_from));

  EXPECT_EQ(moved_from.size(), 0);
  EXPECT_EQ(moved_to.size(), 0);
}

TEST(multiplexing, insert_erase) {
  skynet::poller poller;
  skynet::inet::tcp::socket socket;

  auto ticket = poller.insert(socket, skynet::poller::kPollIn);
  EXPECT_EQ(poller.size(), 1);

  poller.erase(std::move(ticket));

  EXPECT_EQ(poller.size(), 0);
  EXPECT_EQ(socket.family(), skynet::family::kInet);
  EXPECT_EQ(socket.socktype(), skynet::socktype::kStream);
}

TEST(multiplexing, duplicate_insert) {
  skynet::poller poller;
  skynet::inet::tcp::socket socket;

  auto ticket = poller.insert(socket, skynet::poller::kPollIn);

  EXPECT_EQ(poller.size(), 1);
  EXPECT_THROW(std::ignore = poller.insert(socket, skynet::poller::kPollOut),
               std::invalid_argument);

  poller.erase(std::move(ticket));
}

TEST(multiplexing, ticket_reuse) {
  skynet::poller poller;
  skynet::inet::tcp::socket first;
  skynet::inet::tcp::socket second;

  auto first_ticket  = poller.insert(first, skynet::poller::kPollIn);
  auto second_ticket = poller.insert(second, skynet::poller::kPollOut);

  poller.erase(std::move(first_ticket));
  EXPECT_EQ(poller.size(), 1);
  EXPECT_NO_THROW(poller.update(second_ticket, skynet::poller::kPollIn));

  auto third_ticket = poller.insert(first, skynet::poller::kPollPri);
  EXPECT_EQ(poller.size(), 2);

  poller.erase(std::move(second_ticket));
  poller.erase(std::move(third_ticket));
}

}  // namespace tests::unit
