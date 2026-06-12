#pragma once

#include <cerrno>
#include <filesystem>
#include <system_error>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/poll.h>
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

using native_handle_t = int;
using socklen_t       = ::socklen_t;

}  // namespace skynet::platform

/**
 * @section Constants
 */
namespace skynet::platform {

constexpr native_handle_t kInvalidNativeHandle = -1;
constexpr int kErrorRetval                     = -1;

}  // namespace skynet::platform

/**
 * @section Functions
 */
namespace skynet::platform {

constexpr auto create_native_handle(int family, int type, int protocol) noexcept
    -> native_handle_t {
  native_handle_t const handle = ::socket(family, type, protocol);
  if (handle != kInvalidNativeHandle) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    std::ignore = ::fcntl(handle, F_SETFD, FD_CLOEXEC);
  }
  return handle;
}

constexpr auto copy_native_handle(native_handle_t handle) noexcept
    -> native_handle_t {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
  return ::fcntl(handle, F_DUPFD_CLOEXEC);
}

constexpr auto close_native_handle(native_handle_t handle) noexcept -> int {
  return ::close(handle);
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
constexpr auto set_nonblock(native_handle_t handle, int optval) noexcept
    -> int {
  int const flags = ::fcntl(handle, F_GETFL, 0);
  if (flags == platform::kErrorRetval) [[unlikely]] {
    return platform::kErrorRetval;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
  if (::fcntl(handle, F_SETFL,
              // NOLINTNEXTLINE(hicpp-signed-bitwise)
              (optval == 0) ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK)) ==
      platform::kErrorRetval) [[unlikely]] {
    return platform::kErrorRetval;
  }

  return 0;
}

constexpr auto get_nonblock(native_handle_t handle) noexcept -> int {
  // NOLINTNEXTLINE(hicpp-signed-bitwise,cppcoreguidelines-pro-type-vararg)
  return ((::fcntl(handle, F_GETFL) & O_NONBLOCK) != 0) ? 1 : 0;
}

constexpr auto accept(native_handle_t srv_handle) noexcept -> native_handle_t {
  native_handle_t const handle = ::accept(srv_handle, nullptr, nullptr);
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

}  // namespace skynet::platform
