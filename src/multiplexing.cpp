#include <skynet/multiplexing.hpp>

#include <optional>
#include <stdexcept>

#include <swiss/math.hpp>

#include "base_socket_impl.hpp"   // IWYU pragma: keep
#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet {

namespace {

constexpr auto to_native_events(reactor::event_filter filter) noexcept
    -> platform::event_filter_type {
  platform::event_filter_type events{};
  events |= (filter & reactor::kPollIn) != 0 ? platform::kPollIn : 0;
  events |= (filter & reactor::kPollOut) != 0 ? platform::kPollOut : 0;
  events |= (filter & reactor::kPollErr) != 0 ? platform::kPollErr : 0;
  events |= (filter & reactor::kPollHup) != 0 ? platform::kPollHup : 0;
  return events;
}

constexpr auto from_native_events(platform::event_filter_type filter) noexcept
    -> reactor::event_filter {
  std::uint8_t events{};
  events |= (filter & platform::kPollIn) != 0 ? reactor::kPollIn : 0;
  events |= (filter & platform::kPollOut) != 0 ? reactor::kPollOut : 0;
  events |= (filter & platform::kPollErr) != 0 ? reactor::kPollErr : 0;
  events |= (filter & platform::kPollHup) != 0 ? reactor::kPollHup : 0;
  return static_cast<reactor::event_filter>(events);
}

}  // namespace

struct reactor::impl final {
  platform::native_handle_type queue = platform::kInvalidNativeHandle;
  std::vector<platform::event_type> events;
};

auto reactor::event_iterator::operator*() const noexcept
    -> reactor::event_iterator::value_type {
  auto const & native_event = reactor_->pimpl_->events[index_];
  return {
      .filter = from_native_events(platform::get_event_filter(native_event)),
      .data   = platform::get_event_data(native_event),
  };
}

reactor::reactor(std::size_t capacity) {
  auto & [queue, events] = *pimpl_;

  queue = platform::create_reactor();
  if (queue == platform::kInvalidNativeHandle) [[unlikely]] {
    throw std::runtime_error(
        std::format("reactor::reactor(): {}", platform::errno_strerror()));
  }

  events.resize(capacity);
}

reactor::reactor(reactor && that) noexcept { std::swap(pimpl_, that.pimpl_); }

auto reactor::operator=(reactor && that) noexcept -> reactor & {
  std::swap(pimpl_, that.pimpl_);
  return *this;
}

reactor::~reactor() noexcept {
  auto & [queue, _] = *pimpl_;

  if (queue != platform::kInvalidNativeHandle) {
    std::ignore = platform::close_handle(queue);
  }
}

void reactor::insert_or_assign(base_socket const & socket, event_filter filter,
                               void * data) {
  auto & [queue, _] = *pimpl_;

  /**
   * @brief Mask of all input events
   */
  constexpr auto kInputEvents = reactor::kPollIn | reactor::kPollOut;
  if ((filter & ~kInputEvents) != 0) {
    throw std::invalid_argument("reactor::insert_or_assign(): invalid filter");
  }

  if (platform::set_filter(queue, *socket.native_handle(),
                           to_native_events(filter),
                           data) == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(std::format("reactor::insert_or_assign(): {}",
                                         platform::errno_strerror()));
  }
}

void reactor::erase(base_socket const & socket) {
  auto & [queue, _] = *pimpl_;

  if (platform::remove_filter(queue, *socket.native_handle()) ==
      platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("reactor::erase(): {}", platform::errno_strerror()));
  }
}

auto reactor::poll(std::optional<std::chrono::milliseconds> milliseconds)
    -> event_range_type {
  auto & [queue, ready] = *pimpl_;

  auto const retval = platform::poll_reactor(
      queue, std::span<platform::event_type>(ready.data(), ready.size()),
      milliseconds);
  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("reactor::poll(): {}", platform::errno_strerror()));
  }

  auto const size = static_cast<std::size_t>(retval);
  return {event_iterator(this, 0), event_iterator(this, size), size};
}

}  // namespace skynet
