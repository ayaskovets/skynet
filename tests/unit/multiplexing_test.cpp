#include <skynet/inet/sockaddr.hpp>
#include <skynet/inet/tcp/socket.hpp>
#include <skynet/multiplexing.hpp>

#include <gtest/gtest.h>

namespace tests::unit {

TEST(multiplexing, size) {
  static_assert(sizeof(skynet::reactor) == 32);
  static_assert(alignof(skynet::reactor) == 8);
}

TEST(multiplexing, move) {
  skynet::reactor moved_from(1);
  skynet::reactor const moved_to(std::move(moved_from));
}

TEST(multiplexing, insert_erase) {
  skynet::reactor reactor(1);
  skynet::inet::tcp::socket socket;

  reactor.insert_or_assign(socket, skynet::reactor::kPollIn, nullptr);

  reactor.erase(socket);

  EXPECT_EQ(socket.family(), skynet::family::kInet);
  EXPECT_EQ(socket.socktype(), skynet::socktype::kStream);
}

TEST(multiplexing, insert_or_assign) {
  skynet::reactor reactor(1);
  skynet::inet::tcp::socket socket;

  reactor.insert_or_assign(socket, skynet::reactor::kPollIn, nullptr);

  EXPECT_NO_THROW(
      reactor.insert_or_assign(socket, skynet::reactor::kPollOut, nullptr));

  auto const filter = static_cast<skynet::reactor::event_filter>(
      skynet::reactor::kPollIn | skynet::reactor::kPollOut);
  EXPECT_NO_THROW(reactor.insert_or_assign(socket, filter, &socket));
  reactor.insert_or_assign(socket, {}, nullptr);
  EXPECT_NO_THROW(reactor.erase(socket));
}

TEST(multiplexing, invalid_filter) {
  skynet::reactor reactor(1);
  skynet::inet::tcp::socket socket;

  EXPECT_ANY_THROW(
      reactor.insert_or_assign(socket, skynet::reactor::kPollErr, nullptr));
  EXPECT_ANY_THROW(reactor.insert_or_assign(
      socket, static_cast<skynet::reactor::event_filter>(0x80), nullptr));
}

TEST(multiplexing, invalid_socket) {
  skynet::reactor reactor(1);
  skynet::inet::tcp::socket socket(skynet::uninitialized);

  EXPECT_ANY_THROW(
      reactor.insert_or_assign(socket, skynet::reactor::kPollIn, nullptr));
}

TEST(multiplexing, socket_reuse) {
  skynet::reactor reactor(1);
  skynet::inet::tcp::socket first;
  skynet::inet::tcp::socket second;

  reactor.insert_or_assign(first, skynet::reactor::kPollIn, nullptr);
  reactor.insert_or_assign(second, skynet::reactor::kPollOut, nullptr);

  reactor.erase(first);
  EXPECT_NO_THROW(
      reactor.insert_or_assign(second, skynet::reactor::kPollIn, nullptr));

  reactor.insert_or_assign(first, skynet::reactor::kPollIn, nullptr);

  reactor.erase(second);
  reactor.erase(first);
}

TEST(multiplexing, poll_empty) {
  skynet::reactor reactor(1);

  EXPECT_TRUE(reactor.poll(std::chrono::milliseconds(0)).empty());
  auto const events = reactor.poll(std::chrono::milliseconds(1));
  EXPECT_TRUE(events.empty());
  EXPECT_EQ(events.size(), 0);
  EXPECT_EQ(events.begin(), events.end());
}

TEST(multiplexing, poll_nonblocking) {
  skynet::reactor reactor(1);
  skynet::inet::tcp::socket socket;

  reactor.insert_or_assign(socket, skynet::reactor::kPollIn, nullptr);

  auto const events = reactor.poll(std::chrono::milliseconds(0));
  EXPECT_TRUE(events.empty());
  EXPECT_EQ(events.begin(), events.end());

  reactor.erase(socket);
}

TEST(multiplexing, poll_timeout) {
  skynet::reactor reactor(1);
  skynet::inet::tcp::socket socket;

  reactor.insert_or_assign(socket, skynet::reactor::kPollIn, nullptr);

  auto const events = reactor.poll(std::chrono::milliseconds(1));
  EXPECT_TRUE(events.empty());
  EXPECT_EQ(events.begin(), events.end());

  reactor.erase(socket);
}

TEST(multiplexing, poll_write) {
  skynet::reactor reactor(2);
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

  reactor.insert_or_assign(client, skynet::reactor::kPollOut, &client);
  reactor.insert_or_assign(client, {}, nullptr);
  EXPECT_TRUE(reactor.poll(std::chrono::milliseconds(0)).empty());

  auto const filter = static_cast<skynet::reactor::event_filter>(
      skynet::reactor::kPollIn | skynet::reactor::kPollOut);
  reactor.insert_or_assign(client, filter, &client);
  reactor.insert_or_assign(peer, skynet::reactor::kPollIn, &client);
  reactor.insert_or_assign(peer, skynet::reactor::kPollOut, &peer);

  auto const events = reactor.poll(std::nullopt);
  ASSERT_EQ(events.size(), 2);
  EXPECT_NE((*events.begin()).data, (*std::next(events.begin())).data);

  std::size_t count = 0;
  for (auto const event : events) {
    EXPECT_EQ(event.filter, skynet::reactor::kPollOut);
    EXPECT_TRUE(event.data == &client || event.data == &peer);
    ++count;
  }

  EXPECT_EQ(count, 2);

  reactor.erase(client);
  reactor.erase(peer);
  EXPECT_NO_THROW(reactor.erase(peer));
  EXPECT_TRUE(reactor.poll(std::chrono::milliseconds(0)).empty());
}

TEST(multiplexing, poll_read) {
  skynet::reactor reactor(1);
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

  auto const filter = static_cast<skynet::reactor::event_filter>(
      skynet::reactor::kPollIn | skynet::reactor::kPollOut);
  reactor.insert_or_assign(peer, filter, &peer);
  reactor.insert_or_assign(peer, skynet::reactor::kPollIn, &client);
  peer.set_option(skynet::options::nonblock(true));

  std::vector<std::byte> const bytes{std::byte(1), std::byte(2)};
  ASSERT_EQ(client.send(bytes), bytes.size());

  auto const readable = reactor.poll(std::chrono::milliseconds(100));
  ASSERT_EQ(readable.size(), 1);
  EXPECT_EQ((*readable.begin()).filter, skynet::reactor::kPollIn);
  EXPECT_EQ((*readable.begin()).data, &client);

  std::byte buffer{};
  for (auto const byte : bytes) {
    ASSERT_EQ(peer.recv(std::span(&buffer, 1)), 1);
    EXPECT_EQ(buffer, byte);
  }
  EXPECT_EQ(peer.recv(std::span(&buffer, 1)), skynet::base_socket::kWouldBlock);
  EXPECT_TRUE(reactor.poll(std::chrono::milliseconds(0)).empty());

  ASSERT_EQ(client.send(bytes), bytes.size());
  EXPECT_EQ(reactor.poll(std::chrono::milliseconds(100)).size(), 1);
  reactor.erase(peer);
}

}  // namespace tests::unit
