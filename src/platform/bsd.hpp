#pragma once

#include <cerrno>
#include <filesystem>
#include <optional>
#include <span>
#include <system_error>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

/**
 * @section Macros
 */

#ifndef SO_LINGER_SEC
#define SO_LINGER_SEC SO_LINGER
#endif

/**
 * @section Types
 */
namespace skynet::platform {

using native_handle_type = int;
using socklen_t          = ::socklen_t;
using event_filter_type  = std::uint8_t;
using event_type         = struct ::kevent;

}  // namespace skynet::platform

/**
 * @section Constants
 */
namespace skynet::platform {

constexpr native_handle_type kInvalidNativeHandle = -1;
constexpr int kErrorRetval                        = -1;
constexpr event_filter_type kPollin               = 1U << 0U;
constexpr event_filter_type kPollout              = 1U << 1U;
constexpr event_filter_type kPollerr              = 1U << 2U;
constexpr event_filter_type kPollhup              = 1U << 3U;

}  // namespace skynet::platform

/**
 * @section Functions
 */
namespace skynet::platform {

constexpr auto create_socket(int family, int type, int protocol) noexcept
    -> native_handle_type {
  native_handle_type const handle = ::socket(family, type, protocol);
  if (handle != kInvalidNativeHandle) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    std::ignore = ::fcntl(handle, F_SETFD, FD_CLOEXEC);
  }
  return handle;
}

constexpr auto copy_handle(native_handle_type socket) noexcept
    -> native_handle_type {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
  return ::fcntl(socket, F_DUPFD_CLOEXEC, 0);
}

constexpr auto close_handle(native_handle_type socket) noexcept -> int {
  return ::close(socket);
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
constexpr auto set_nonblock(native_handle_type socket, int optval) noexcept
    -> int {
  int const flags = ::fcntl(socket, F_GETFL, 0);
  if (flags == platform::kErrorRetval) [[unlikely]] {
    return platform::kErrorRetval;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
  if (::fcntl(socket, F_SETFL,
              (optval == 0) ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK)) ==
      platform::kErrorRetval) [[unlikely]] {
    return platform::kErrorRetval;
  }

  return 0;
}

constexpr auto get_nonblock(native_handle_type socket) noexcept -> int {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
  return ((::fcntl(socket, F_GETFL) & O_NONBLOCK) != 0) ? 1 : 0;
}

constexpr auto accept(native_handle_type socket) noexcept
    -> native_handle_type {
  native_handle_type const handle = ::accept(socket, nullptr, nullptr);
  if (handle != kInvalidNativeHandle) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    std::ignore = ::fcntl(handle, F_SETFD, FD_CLOEXEC);
  }
  return handle;
}

constexpr auto unlink(std::filesystem::path const & path) noexcept -> int {
  return ::unlink(path.c_str());
}

constexpr auto get_last_error() noexcept -> std::errc {
  return std::errc(errno);
}

constexpr auto errno_strerror() noexcept -> std::string {
  return std::error_code(
             static_cast<std::underlying_type_t<std::errc>>(std::errc(errno)),
             std::generic_category())
      .message();
}

constexpr auto create_poller() noexcept -> native_handle_type {
  native_handle_type const poller = ::kqueue();
  if (poller != kInvalidNativeHandle) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    std::ignore = ::fcntl(poller, F_SETFD, FD_CLOEXEC);
  }
  return poller;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
constexpr auto set_filter(native_handle_type poller, native_handle_type socket,
                          event_filter_type filter, void * data) noexcept
    -> int {
  struct ::kevent event{};
  switch (filter) {
    case kPollin:
      EV_SET(&event, socket, EVFILT_READ, EV_ADD, 0, 0, data);
      break;
    case kPollout:
      EV_SET(&event, socket, EVFILT_WRITE, EV_ADD, 0, 0, data);
      break;
    default:
      errno = EINVAL;
      return kErrorRetval;
  }
  return ::kevent(poller, &event, 1, nullptr, 0, nullptr);
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
constexpr auto remove_filter(native_handle_type poller,
                             native_handle_type socket,
                             event_filter_type filter) noexcept -> int {
  struct ::kevent event{};
  switch (filter) {
    case kPollin:
      EV_SET(&event, socket, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
      break;
    case kPollout:
      EV_SET(&event, socket, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
      break;
    default:
      errno = EINVAL;
      return kErrorRetval;
  }

  auto const retval = ::kevent(poller, &event, 1, nullptr, 0, nullptr);
  if (retval == kErrorRetval && errno == ENOENT) {
    /**
     * @note Ignore ENOENT to allow silently dropping errors due to removing
     * non-registered filters
     */
    return 0;
  }

  return retval;
}

constexpr auto wait_poller(
    native_handle_type poller, std::span<event_type> events,
    std::optional<std::chrono::milliseconds> timeout) noexcept -> int {
  if (timeout.has_value()) {
    auto const seconds =
        std::chrono::duration_cast<std::chrono::seconds>(*timeout);

    ::timespec const timespec{
        .tv_sec  = seconds.count(),
        .tv_nsec = std::chrono::nanoseconds(*timeout - seconds).count(),
    };

    return ::kevent(poller, nullptr, 0, events.data(),
                    static_cast<int>(events.size()), &timespec);
  }

  return ::kevent(poller, nullptr, 0, events.data(),
                  static_cast<int>(events.size()), nullptr);
}

constexpr auto get_event_filter(event_type event) noexcept
    -> event_filter_type {
  event_filter_type filter{};
  filter |= event.filter == EVFILT_READ ? kPollin : 0;
  filter |= event.filter == EVFILT_WRITE ? kPollout : 0;
  filter |= ((event.flags & EV_ERROR) != 0 ||
             ((event.flags & EV_EOF) != 0 && event.fflags != 0))
                ? kPollerr
                : 0;
  filter |= (event.flags & EV_EOF) != 0 ? kPollhup : 0;
  return filter;
}

constexpr auto get_event_data(event_type event) noexcept -> void * {
  return event.udata;
}

}  // namespace skynet::platform
