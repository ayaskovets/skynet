#include <skynet/multiplexing.hpp>

#include <stdexcept>

#include <swiss/math.hpp>

#include "base_socket_impl.hpp"   // IWYU pragma: keep
#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet {

namespace {

constexpr auto to_native_events(poller::event_filter filter) noexcept
    -> platform::event_filter_type {
  switch (filter) {
    case poller::kPollIn:
      return platform::kPollin;
    case poller::kPollOut:
      return platform::kPollout;
    case poller::kPollErr:
      return platform::kPollerr;
    case poller::kPollHup:
      return platform::kPollhup;
  }
  std::unreachable();
}

[[maybe_unused]] constexpr auto from_native_events(
    platform::event_filter_type filter) noexcept -> poller::event_filter {
  // NOLINTNEXTLINE(bugprone-switch-missing-default-case)
  switch (filter) {
    case platform::kPollin:
      return poller::kPollIn;
    case platform::kPollout:
      return poller::kPollOut;
    case platform::kPollerr:
      return poller::kPollErr;
    case platform::kPollhup:
      return poller::kPollHup;
  }
  std::unreachable();
}

}  // namespace

struct poller::impl final {
  platform::native_handle_type queue = platform::kInvalidNativeHandle;
  std::vector<platform::event_type> events;
};

poller::poller(std::size_t capacity) {
  auto & [queue, events] = *pimpl_;

  queue = platform::create_poller();
  if (queue == platform::kInvalidNativeHandle) [[unlikely]] {
    throw std::runtime_error(
        std::format("poller::poller(): {}", platform::errno_strerror()));
  }

  events.resize(capacity);
}

poller::poller(poller && that) noexcept { std::swap(pimpl_, that.pimpl_); }

auto poller::operator=(poller && that) noexcept -> poller & {
  std::swap(pimpl_, that.pimpl_);
  return *this;
}

poller::~poller() noexcept {
  auto & [queue, _] = *pimpl_;

  if (queue != platform::kInvalidNativeHandle) {
    std::ignore = platform::close_handle(queue);
  }
}

void poller::insert_or_assign(base_socket const & socket, event_filter filter,
                              void * data) {
  auto & [queue, _] = *pimpl_;

  for (event_filter const event : {kPollIn, kPollOut}) {
    if ((filter & event) == 0) {
      continue;
    }

    if (platform::set_filter(queue, *socket.native_handle(),
                             to_native_events(event),
                             data) == platform::kErrorRetval) [[unlikely]] {
      throw std::runtime_error(std::format("poller::insert_or_assign(): {}",
                                           platform::errno_strerror()));
    }
  }
}

void poller::erase(base_socket const & socket) {
  auto & [queue, _] = *pimpl_;

  if (platform::remove_filter(queue, *socket.native_handle()) ==
      platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("poller::erase(): {}", platform::errno_strerror()));
  }
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
  auto & [queue, ready] = *pimpl_;

  auto const retval = platform::wait_poller(
      queue, std::span<platform::event_type>(ready.data(), ready.size()),
      milliseconds);
  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("poller::poll_for(): {}", platform::errno_strerror()));
  }

  for (std::size_t i = 0; i < static_cast<std::size_t>(retval); ++i) {
    // TODO: handle socket, resize ready
  }
}

}  // namespace skynet
