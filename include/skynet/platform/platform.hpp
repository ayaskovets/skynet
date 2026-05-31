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
concept _public_platform = requires {
  /**
   * @section Types
   */

  /**
   * @section Constants
   */
  { kNativeHandleSize } -> swiss::decay_same_as<std::size_t>;
  { kNativeHandleAlignment } -> swiss::decay_same_as<std::size_t>;

  /**
   * @section Functions
   */
};
static_assert(_public_platform<>, "invalid platform implementation");

}  // namespace skynet::platform
