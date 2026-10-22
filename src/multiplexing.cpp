#include <skynet/multiplexing.hpp>

#include <stdexcept>

#include <swiss/math.hpp>

#include "base_socket_impl.hpp"   // IWYU pragma: keep
#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet {

namespace {
// NOLINTBEGIN(hicpp-signed-bitwise)

constexpr auto to_native_events(poller::events_type events) noexcept
    -> std::int16_t {
  std::int16_t native_events{};
  native_events |= ((events & poller::event::kPollIn) != 0) ? POLLIN : 0;
  native_events |= ((events & poller::event::kPollPri) != 0) ? POLLPRI : 0;
  native_events |= ((events & poller::event::kPollOut) != 0) ? POLLOUT : 0;
  native_events |= ((events & poller::event::kPollErr) != 0) ? POLLERR : 0;
  native_events |= ((events & poller::event::kPollHup) != 0) ? POLLHUP : 0;
  native_events |= ((events & poller::event::kPollNval) != 0) ? POLLNVAL : 0;
  return native_events;
}

[[maybe_unused]] constexpr auto from_native_events(
    std::int16_t native_events) noexcept
    -> poller::events_type {
  poller::events_type events{};
  events |= ((native_events & POLLIN) != 0) ? poller::event::kPollIn : 0;
  events |= ((native_events & POLLPRI) != 0) ? poller::event::kPollPri : 0;
  events |= ((native_events & POLLOUT) != 0) ? poller::event::kPollOut : 0;
  events |= ((native_events & POLLERR) != 0) ? poller::event::kPollErr : 0;
  events |= ((native_events & POLLHUP) != 0) ? poller::event::kPollHup : 0;
  events |= ((native_events & POLLNVAL) != 0) ? poller::event::kPollNval : 0;
  return events;
}

// NOLINTEND(hicpp-signed-bitwise)
}  // namespace

struct poller::impl final {
  std::vector<platform::pollfd> fds;
  std::vector<std::size_t> softdel;
};

poller::poller() noexcept = default;

poller::poller(poller && that) noexcept = default;

auto poller::operator=(poller && that) noexcept -> poller & = default;

poller::~poller() noexcept = default;

auto poller::capacity() const noexcept -> std::size_t {
  auto const & [polling, _] = *pimpl_;

  return polling.capacity();
}

auto poller::size() const noexcept -> std::size_t {
  auto const & [polling, softdel] = *pimpl_;

  return polling.size() - softdel.size();
}

void poller::shrink_to_fit() noexcept {
  auto & [polling, softdel] = *pimpl_;

  polling.shrink_to_fit();
  softdel.shrink_to_fit();
}

auto poller::insert(base_socket const & sock, events_type events) -> ticket {
  auto & [polling, softdel] = *pimpl_;

  for (auto const & pollfd : polling) {
    if (pollfd.fd != platform::kInvalidNativeHandle &&
        pollfd.fd == *sock.native_handle()) {
      throw std::invalid_argument("poller::insert(): socket already inserted");
    }
  }

  platform::pollfd const pollfd{
      .fd      = *sock.native_handle(),
      .events  = to_native_events(events),
      .revents = {},
  };

  if (softdel.empty()) {
    polling.push_back(pollfd);
    return ticket(polling.size() - 1);
  }

  std::size_t const index = softdel.back();
  softdel.pop_back();
  polling[index] = pollfd;
  return ticket(index);
}

void poller::update(ticket const & ticket, events_type events) {
  if (ticket.is_moved_from()) {
    throw std::invalid_argument("poller::erase(): reused ticket");
  }

  auto & [polling, _] = *pimpl_;

  polling[ticket.index_].events = to_native_events(events);
}

void poller::erase(ticket ticket) {
  if (ticket.is_moved_from()) {
    throw std::invalid_argument("poller::erase(): reused ticket");
  }

  auto & [polling, softdel] = *pimpl_;

  if (ticket.index_ >= polling.size() ||
      polling[ticket.index_].fd == platform::kInvalidNativeHandle) {
    throw std::invalid_argument("poller::erase(): invalid ticket");
  }

  polling[ticket.index_] = platform::pollfd{
      .fd      = platform::kInvalidNativeHandle,
      .events  = {},
      .revents = {},
  };
  softdel.push_back(ticket.index_);
}

void poller::try_poll() {
  constexpr std::chrono::milliseconds kNonBlockingPollTimeout(0);
  poll_for(kNonBlockingPollTimeout);
}

void poller::poll() {
  constexpr std::chrono::milliseconds kBlockingTimeout(-1);
  poll_for(kBlockingTimeout);
}

void poller::poll_for(std::chrono::milliseconds milliseconds) {
  auto & [polling, softdel] = *pimpl_;

  auto const retval = platform::poll(
      polling.data(), static_cast<platform::nfds_t>(polling.size()),
      swiss::bound<int>(milliseconds.count()));
  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("poller::poll_for(): {}", platform::errno_strerror()));
  }

  for (std::size_t i = 0; i < static_cast<std::size_t>(retval); ++i) {
    if (polling[i].revents == to_native_events({})) {
      continue;
    }

    /**
     * @note Manually reset socket handle to pass a const reference to a socket
     * object
     */
    base_socket socket(uninitialized);
    *socket.native_handle() = polling[i].fd;

    // TODO: handle socket
  }
}

}  // namespace skynet
