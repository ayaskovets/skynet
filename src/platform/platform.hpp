#pragma once

#include <swiss/concepts.hpp>
#include <swiss/os_kind.hpp>

/**
 * @brief Platform-specific types, functions and variables are implicitly used
 * throughout the library code and have to be defined in the corresponding
 * platform header. The implementation must reside in the 'skynet::platform'
 * namespace
 */
#if (SWISS_OS_KIND == kLinux) || (SWISS_OS_KIND == kUnix) || \
    (SWISS_OS_KIND == kMacOS)
#include "unix.hpp"
#endif

namespace skynet::platform {

template <typename Platform = void>
concept _private_platform = requires(native_handle_t handle, int family,
                                     int type, int protocol, int optval) {
  /**
   * @section Types
   */
  { handle } -> swiss::not_same_as<void>;

  /**
   * @section Constants
   */
  { kInvalidNativeHandle } -> swiss::decay_same_as<native_handle_t>;
  { kErrorRetval } -> swiss::decay_same_as<int>;

  /**
   * @section Functions
   */
  {
    create_native_handle(family, type, protocol)
  } -> std::same_as<native_handle_t>;
  { copy_native_handle(handle) } -> std::same_as<int>;
  { close_native_handle(handle) } -> std::same_as<int>;
  { set_nonblock(handle, optval) } -> std::same_as<int>;
  { get_nonblock(handle) } -> std::same_as<int>;
  { accept(handle) } -> std::same_as<native_handle_t>;
  { get_last_error() } -> std::same_as<std::errc>;
  { errno_strerror() } -> std::same_as<std::string>;

  /**
   * @section Exceptions
   */
  requires noexcept(create_native_handle(family, type, protocol));
  requires noexcept(close_native_handle(handle));
  requires noexcept(copy_native_handle(handle));
  requires noexcept(set_nonblock(handle, optval));
  requires noexcept(get_nonblock(handle));
  requires noexcept(accept(handle));
  requires noexcept(get_last_error());
  requires noexcept(errno_strerror());
};
static_assert(_private_platform<>, "invalid platform implementation");

}  // namespace skynet::platform
