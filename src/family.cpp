#include <skynet/family.hpp>

#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet {

auto to_native_family(family family) noexcept -> int {
  switch (family) {
    case family::kUnspec:
      return AF_UNSPEC;
    case family::kInet:
      return AF_INET;
    case family::kInet6:
      return AF_INET6;
    case family::kUnix:
      return AF_UNIX;
  }
}

auto from_native_family(int native_family) -> family {
  switch (native_family) {
    case AF_INET:
      return family::kInet;
    case AF_INET6:
      return family::kInet6;
    case AF_UNIX:
      return family::kUnix;
    [[unlikely]] default:
      throw std::invalid_argument(
          std::format("invalid/unimplemented native family {}", native_family));
  }
}

}  // namespace skynet
