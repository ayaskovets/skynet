#include <skynet/inet6/sockaddr.hpp>

#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet::inet6 {
// NOLINTBEGIN(bugprone-casting-through-void)

// NOLINTNEXTLINE(readability-identifier-length)
sockaddr::sockaddr(inet6::ip ip, inet6::port port)
    : base_sockaddr(family::kInet6) {
  auto * storage =
      static_cast<sockaddr_in6 *>(base_sockaddr::native_sockaddr());
  std::memcpy(&storage->sin6_addr, ip.to_bytes(network_byte_order_t{}).data(),
              sizeof(storage->sin6_addr));
  storage->sin6_port = port.to_bytes(network_byte_order_t{});
}

auto sockaddr::ip() const noexcept -> inet6::ip {
  auto const * storage =
      static_cast<sockaddr_in6 const *>(base_sockaddr::native_sockaddr());
  return inet6::ip(
      network_byte_order_t{},
      std::bit_cast<std::array<std::byte, sizeof(storage->sin6_addr)>>(
          storage->sin6_addr));
}

auto sockaddr::port() const noexcept -> inet6::port {
  auto const * storage =
      static_cast<sockaddr_in6 const *>(base_sockaddr::native_sockaddr());
  return inet6::port(network_byte_order_t{}, storage->sin6_port);
}

// NOLINTEND(bugprone-casting-through-void)
}  // namespace skynet::inet6
