#pragma once

#include <filesystem>

#include <swiss/concepts.hpp>
#include <swiss/os_kind.hpp>

/**
 * @brief Platform-specific types, functions and variables are implicitly used
 * throughout the library code and have to be defined in the corresponding
 * platform header. The implementation must reside in the 'skynet::platform'
 * namespace
 */
#if (SWISS_OS_KIND == kMacOS)
#include "bsd.hpp"
#endif

namespace skynet::platform::concepts {

template <typename Platform = void>
concept is_valid_platform_implementaiton = requires(
    platform::native_handle_type handle, platform::socklen_t socklen,
    platform::pollfd pollfd, platform::nfds_t nfds, int family, int type,
    int protocol, int optval, int timeout, std::filesystem::path path) {
  /**
   * @section Types
   */
  { handle } -> swiss::not_same_as<void>;
  { socklen } -> swiss::not_same_as<void>;
  { pollfd } -> swiss::not_same_as<void>;
  { nfds } -> swiss::not_same_as<void>;

  /**
   * @section Constants
   */
  {
    platform::kInvalidNativeHandle
  } -> swiss::decay_same_as<native_handle_type>;
  { platform::kErrorRetval } -> swiss::decay_same_as<int>;

  /**
   * @section Functions
   */
  {
    platform::create_native_handle(family, type, protocol)
  } -> std::same_as<native_handle_type>;
  { platform::copy_native_handle(handle) } -> std::same_as<int>;
  { platform::close_native_handle(handle) } -> std::same_as<int>;
  { platform::set_nonblock(handle, optval) } -> std::same_as<int>;
  { platform::get_nonblock(handle) } -> std::same_as<int>;
  { platform::accept(handle) } -> std::same_as<native_handle_type>;
  { platform::poll(&pollfd, nfds, timeout) } -> std::same_as<int>;
  { platform::unlink(path) } -> std::same_as<int>;
  { platform::get_last_error() } -> std::same_as<std::errc>;
  { platform::errno_strerror() } -> std::same_as<std::string>;

  /**
   * @section Exceptions
   */
  requires noexcept(platform::create_native_handle(family, type, protocol));
  requires noexcept(platform::close_native_handle(handle));
  requires noexcept(platform::copy_native_handle(handle));
  requires noexcept(platform::set_nonblock(handle, optval));
  requires noexcept(platform::get_nonblock(handle));
  requires noexcept(platform::accept(handle));
  requires noexcept(platform::poll(&pollfd, nfds, timeout));
  requires noexcept(platform::unlink(path));
  requires noexcept(platform::get_last_error());
  requires noexcept(platform::errno_strerror());
};
static_assert(is_valid_platform_implementaiton<>,
              "invalid platform implementation");

}  // namespace skynet::platform::concepts
