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
concept _private_platform = requires(native_handle handle) {
  /**
   * @section Types
   */
  { handle } -> swiss::not_same_as<void>;

  /**
   * @section Constants
   */
  { kInvalidNativeHandle } -> swiss::decay_same_as<native_handle>;
  { kErrorRetval } -> swiss::decay_same_as<int>;

  /**
   * @section Functions
   */
  { close_native_handle(handle) } -> std::same_as<int>;
  { get_last_error() } -> std::same_as<std::errc>;
  { errno_strerror() } -> std::same_as<std::string>;
};
static_assert(_private_platform<>, "invalid platform implementation");

}  // namespace skynet::platform
