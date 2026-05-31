#pragma once

#include <swiss/os_kind.hpp>

/**
 * @brief Platform-specific implementation are implicitly used in the library
 * code and have to be defined in the corresponding platform header. The
 * implementation is expected to be defined inside a class named exactly
 * 'platform' and has to conform to the concept defined below
 */
#if (SWISS_OS_KIND == kLinux) || (SWISS_OS_KIND == kUnix) || \
    (SWISS_OS_KIND == kMacOS)
#include "unix.hpp"
#endif

namespace skynet::platform {

template <typename Platform = void>
concept is_platform = requires {
  { errno_strerror() } -> std::same_as<std::string>;
};
static_assert(is_platform<>, "invalid platform implementation");

}  // namespace skynet::platform
