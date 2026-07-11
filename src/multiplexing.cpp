#include <skynet/multiplexing.hpp>

#include <stdexcept>

#include <swiss/math.hpp>

#include "base_socket_impl.hpp"   // IWYU pragma: keep
#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet {

namespace {
// NOLINTBEGIN(hicpp-signed-bitwise)

constexpr auto to_native_events(poller::events_t events) noexcept
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

constexpr auto from_native_events(std::int16_t native_events) noexcept
    -> poller::events_t {
  poller::events_t events{};
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

class poller::impl final {
 public:
  std::vector<::pollfd> polling;
  std::vector<::pollfd> pending;
};

poller::poller(callback_t callback) : callback_(std::move(callback)) {
  if (!callback_) [[unlikely]] {
    throw std::invalid_argument("poller::poller(): null callback");
  }
}

poller::poller(poller &&) noexcept = default;

auto poller::operator=(poller &&) noexcept -> poller & = default;

poller::~poller() noexcept {
  auto const & [polling, pending] = *pimpl_;

  for (::pollfd const pollfd : polling) {
    base_socket sock(uninitialized_t{});
    *sock.native_handle() = pollfd.fd;
  }

  for (::pollfd const pollfd : pending) {
    base_socket sock(uninitialized_t{});
    *sock.native_handle() = pollfd.fd;
  }
}

auto poller::capacity() const noexcept -> std::size_t {
  auto const & [polling, pending] = *pimpl_;

  return polling.capacity() + pending.capacity();
}

auto poller::size() const noexcept -> std::size_t {
  auto const & [polling, pending] = *pimpl_;

  return polling.size() + pending.size();
}

void poller::insert(base_socket sock, events_t events) {
  auto & [polling, pending] = *pimpl_;

  ::pollfd const pollfd{
      .fd      = *sock.native_handle(),
      .events  = to_native_events(events),
      .revents = {},
  };

  if (polling.size() < polling.capacity()) {
    polling.push_back(pollfd);
  } else {
    pending.push_back(pollfd);
  }

  /**
   * @note Manually reset socket handle so it does not get closed
   */
  *sock.native_handle() = platform::kInvalidNativeHandle;
}

void poller::shrink_to_fit() {
  auto & [polling, pending] = *pimpl_;

  if (!pending.empty()) {
    polling.reserve(polling.size() + pending.size());
    polling.insert_range(polling.end(), pending);
    pending.clear();
  }

  polling.shrink_to_fit();
  pending.shrink_to_fit();
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
  auto & [polling, pending] = *pimpl_;

  if (!pending.empty()) {
    polling.reserve(polling.size() + pending.size());
    polling.insert_range(polling.end(), pending);
    pending.clear();
  }

  auto const retval =
      ::poll(polling.data(), static_cast<::nfds_t>(polling.size()),
             swiss::bound<int>(milliseconds.count()));
  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("poller::poll_for(): {}", platform::errno_strerror()));
  }

  for (std::size_t i = 0;
       i < static_cast<std::size_t>(retval) && i < polling.size();) {
    if (polling[i].revents == to_native_events({})) {
      continue;
    }

    /**
     * @note Manually reset socket handle
     */
    base_socket socket(uninitialized_t{});
    *socket.native_handle() = polling[i].fd;

    callback_result const cb_result =
        callback_(socket, from_native_events(polling[i].revents));

    switch (cb_result) {
      case callback_result::kClose:
        /**
         * @note Socket gets closed at the end of iteration
         */
        polling.erase(
            std::next(polling.begin(), static_cast<std::ptrdiff_t>(i)));
        break;
      case callback_result::kDoNothing:
        *socket.native_handle() = platform::kInvalidNativeHandle;
        break;
    }

    i -= 1;
  }
}

}  // namespace skynet
