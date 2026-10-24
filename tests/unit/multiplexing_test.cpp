#include <skynet/inet/sockaddr.hpp>
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

  poller.erase(socket, skynet::poller::kPollIn);

  EXPECT_EQ(socket.family(), skynet::family::kInet);
  EXPECT_EQ(socket.socktype(), skynet::socktype::kStream);
}

TEST(multiplexing, insert_or_assign) {
  skynet::poller poller(1);
  skynet::inet::tcp::socket socket;

  poller.insert_or_assign(socket, skynet::poller::kPollIn, nullptr);

  EXPECT_NO_THROW(
      poller.insert_or_assign(socket, skynet::poller::kPollOut, nullptr));

  auto const filter = static_cast<skynet::poller::event_filter>(
      skynet::poller::kPollIn | skynet::poller::kPollOut);
  poller.erase(socket, filter);
}

TEST(multiplexing, socket_reuse) {
  skynet::poller poller(1);
  skynet::inet::tcp::socket first;
  skynet::inet::tcp::socket second;

  poller.insert_or_assign(first, skynet::poller::kPollIn, nullptr);
  poller.insert_or_assign(second, skynet::poller::kPollOut, nullptr);

  poller.erase(first, skynet::poller::kPollIn);
  EXPECT_NO_THROW(
      poller.insert_or_assign(second, skynet::poller::kPollIn, nullptr));

  poller.insert_or_assign(first, skynet::poller::kPollIn, nullptr);

  auto const filter = static_cast<skynet::poller::event_filter>(
      skynet::poller::kPollIn | skynet::poller::kPollOut);
  poller.erase(second, filter);
  poller.erase(first, skynet::poller::kPollIn);
}

TEST(multiplexing, poll_empty) {
  skynet::poller poller(1);

  EXPECT_TRUE(poller.poll(std::chrono::milliseconds(0)).empty());
  auto const events = poller.poll(std::chrono::milliseconds(1));
  EXPECT_TRUE(events.empty());
  EXPECT_EQ(events.size(), 0);
  EXPECT_EQ(events.begin(), events.end());
}

TEST(multiplexing, poll_nonblocking) {
  skynet::poller poller(1);
  skynet::inet::tcp::socket socket;

  poller.insert_or_assign(socket, skynet::poller::kPollIn, nullptr);

  auto const events = poller.poll(std::chrono::milliseconds(0));
  EXPECT_TRUE(events.empty());
  EXPECT_EQ(events.begin(), events.end());

  poller.erase(socket, skynet::poller::kPollIn);
}

TEST(multiplexing, poll_timeout) {
  skynet::poller poller(1);
  skynet::inet::tcp::socket socket;

  poller.insert_or_assign(socket, skynet::poller::kPollIn, nullptr);

  auto const events = poller.poll(std::chrono::milliseconds(1));
  EXPECT_TRUE(events.empty());
  EXPECT_EQ(events.begin(), events.end());

  poller.erase(socket, skynet::poller::kPollIn);
}

TEST(multiplexing, poll) {
  skynet::poller poller(2);
  skynet::inet::tcp::socket listener;
  skynet::inet::tcp::socket client;
  skynet::inet::tcp::socket peer(skynet::uninitialized);
  skynet::inet::sockaddr address(skynet::inet::ip::kLoopback(),
                                 skynet::inet::port(0));

  ASSERT_EQ(listener.bind(address), skynet::bind_status::kSuccess);
  listener.getsockname(address);
  listener.listen(1);
  ASSERT_EQ(client.connect(address), skynet::connect_status::kSuccess);
  ASSERT_EQ(listener.accept(peer), skynet::accept_status::kSuccess);

  poller.insert_or_assign(client, skynet::poller::kPollOut, &client);
  poller.insert_or_assign(peer, skynet::poller::kPollOut, &peer);
  poller.insert_or_assign(peer, skynet::poller::kPollIn, &client);
  poller.erase(peer, skynet::poller::kPollIn);

  auto const events = poller.poll(std::nullopt);
  ASSERT_EQ(events.size(), 2);
  EXPECT_NE((*events.begin()).data, (*std::next(events.begin())).data);

  std::size_t count = 0;
  for (auto const event : events) {
    EXPECT_EQ(event.filter, skynet::poller::kPollOut);
    EXPECT_TRUE(event.data == &client || event.data == &peer);
    ++count;
  }

  EXPECT_EQ(count, 2);

  auto const filter = static_cast<skynet::poller::event_filter>(
      skynet::poller::kPollIn | skynet::poller::kPollOut);
  poller.erase(client, filter);
  poller.erase(peer, skynet::poller::kPollOut);
  EXPECT_NO_THROW(poller.erase(peer, skynet::poller::kPollOut));
}

}  // namespace tests::unit
