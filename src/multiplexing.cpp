#include <skynet/multiplexing.hpp>

#include <optional>
#include <stdexcept>

#include <swiss/math.hpp>

#include "base_socket_impl.hpp"   // IWYU pragma: keep
#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet {

namespace {

/**
 * @brief List of all input events
 */
constexpr auto kInputEvents = {poller::kPollIn, poller::kPollOut};

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
}

constexpr auto from_native_events(platform::event_filter_type filter) noexcept
    -> poller::event_filter {
  std::uint8_t events{};
  events |= (filter & platform::kPollin) != 0 ? poller::kPollIn : 0;
  events |= (filter & platform::kPollout) != 0 ? poller::kPollOut : 0;
  events |= (filter & platform::kPollerr) != 0 ? poller::kPollErr : 0;
  events |= (filter & platform::kPollhup) != 0 ? poller::kPollHup : 0;
  return static_cast<poller::event_filter>(events);
}

}  // namespace

struct poller::impl final {
  platform::native_handle_type queue = platform::kInvalidNativeHandle;
  std::vector<platform::event_type> events;
};

auto poller::event_iterator::operator*() const noexcept
    -> poller::event_iterator::value_type {
  auto const & native_event = poller_->pimpl_->events[index_];
  return {
      .filter = from_native_events(platform::get_event_filter(native_event)),
      .data   = platform::get_event_data(native_event),
  };
}

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

  for (event_filter const event : kInputEvents) {
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

void poller::erase(base_socket const & socket, event_filter filter) {
  auto & [queue, _] = *pimpl_;

  for (event_filter const event : kInputEvents) {
    if ((filter & event) == 0) {
      continue;
    }

    if (platform::remove_filter(queue, *socket.native_handle(),
                                to_native_events(event)) ==
        platform::kErrorRetval) [[unlikely]] {
      throw std::runtime_error(
          std::format("poller::erase(): {}", platform::errno_strerror()));
    }
  }
}

auto poller::poll(std::optional<std::chrono::milliseconds> milliseconds)
    -> event_range_type {
  auto & [queue, ready] = *pimpl_;

  auto const retval = platform::wait_poller(
      queue, std::span<platform::event_type>(ready.data(), ready.size()),
      milliseconds);
  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("poller::poll_for(): {}", platform::errno_strerror()));
  }

  auto const size = static_cast<std::size_t>(retval);
  return {event_iterator(this, 0), event_iterator(this, size), size};
}

}  // namespace skynet
