#pragma once

#include <cstddef>

/**
 * @section Constants
 */
namespace skynet::platform {

constexpr std::size_t kNativeHandleSize      = sizeof(int);
constexpr std::size_t kNativeHandleAlignment = alignof(int);

}  // namespace skynet::platform
