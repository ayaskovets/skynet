#pragma once

#include <cerrno>
#include <system_error>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

/**
 * @section Types
 */
namespace skynet::platform {

using native_handle = int;

}  // namespace skynet::platform

/**
 * @section Constants
 */
namespace skynet::platform {

constexpr int kInvalidSocket = -1;
constexpr int kErrorRetval   = -1;

}  // namespace skynet::platform

/**
 * @section Functions
 */
namespace skynet::platform {

constexpr auto close_native_handle(int native_handle) noexcept -> int {
  return ::close(native_handle);
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
