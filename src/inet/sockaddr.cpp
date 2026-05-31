#include <skynet/inet/sockaddr.hpp>

#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet::inet {
// NOLINTBEGIN(bugprone-casting-through-void)

// NOLINTNEXTLINE(readability-identifier-length)
sockaddr::sockaddr(inet::ip ip, inet::port port)
    : base_sockaddr(family::kInet) {
  auto * storage = static_cast<sockaddr_in *>(base_sockaddr::native_sockaddr());
  storage->sin_addr.s_addr = ip.to_bytes(network_byte_order_t{});
  storage->sin_port        = port.to_bytes(network_byte_order_t{});
}

auto sockaddr::ip() const noexcept -> inet::ip {
  auto const * storage =
      static_cast<sockaddr_in const *>(base_sockaddr::native_sockaddr());
  return inet::ip(network_byte_order_t{}, storage->sin_addr.s_addr);
}

auto sockaddr::port() const noexcept -> inet::port {
  auto const * storage =
      static_cast<sockaddr_in const *>(base_sockaddr::native_sockaddr());
  return inet::port(network_byte_order_t{}, storage->sin_port);
}

// NOLINTEND(bugprone-casting-through-void)
}  // namespace skynet::inet
