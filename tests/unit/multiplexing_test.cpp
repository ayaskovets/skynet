#include <skynet/inet/tcp/socket.hpp>
#include <skynet/multiplexing.hpp>
#include <skynet/options.hpp>

#include <gtest/gtest.h>
#include <swiss/functional.hpp>

#include "skynet/inet/sockaddr.hpp"
#include "skynet/status.hpp"

namespace tests::unit {

TEST(multiplexing, size) {
  static_assert(sizeof(skynet::poller) == 80);
  static_assert(alignof(skynet::poller) == 8);
}

TEST(multiplexing, move) {
  std::size_t calls = 0;
  skynet::poller moved_from(
      [&calls](skynet::base_socket const &, skynet::poller::events_t) noexcept {
        calls += 1;
        return skynet::poller::callback_result::kDoNothing;
      });
  skynet::poller const moved_to(std::move(moved_from));

  EXPECT_EQ(moved_from.size(), 0);
  EXPECT_EQ(moved_to.size(), 0);
  EXPECT_EQ(calls, 0);
}

TEST(multiplexing, callback_smoke) {
  std::size_t calls = 0;
  skynet::poller poller([&calls](skynet::base_socket const & sock,
                                 skynet::poller::events_t) noexcept {
    EXPECT_EQ(sock.family(), skynet::family::kInet);
    EXPECT_EQ(sock.socktype(), skynet::socktype::kStream);
    calls += 1;
    return skynet::poller::callback_result::kClose;
  });

  skynet::inet::sockaddr const sockaddr(skynet::inet::ip::kLoopback(),
                                        skynet::inet::port(9999));

  skynet::inet::tcp::socket server;
  server.set_option(skynet::reuseaddr(true));
  server.set_option(skynet::nonblock(true));
  server.bind(sockaddr);
  server.listen(1);

  skynet::inet::tcp::socket client;
  client.set_option(skynet::nonblock(true));

  poller.insert(std::move(server), skynet::poller::kPollIn);
  EXPECT_EQ(poller.size(), 1);

  EXPECT_EQ(client.connect(sockaddr), skynet::connect_status::kPending);
  poller.poll();

  EXPECT_EQ(poller.size(), 0);
  EXPECT_EQ(calls, 1);
}

TEST(multiplexing, destructor) {
  std::size_t calls = 0;
  skynet::poller poller(
      [&calls](skynet::base_socket const &, skynet::poller::events_t) noexcept {
        calls += 1;
        return skynet::poller::callback_result::kDoNothing;
      });

  skynet::inet::sockaddr const sockaddr(skynet::inet::ip::kLoopback(),
                                        skynet::inet::port(9999));

  skynet::inet::tcp::socket server;
  poller.insert(std::move(server), skynet::poller::kPollIn);

  EXPECT_EQ(poller.size(), 1);
  EXPECT_EQ(calls, 0);
}

}  // namespace tests::unit