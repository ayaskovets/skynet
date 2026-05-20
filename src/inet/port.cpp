#include <skynet/inet/port.hpp>

#include <arpa/inet.h>

namespace skynet::inet {

port::port(std::uint16_t port) noexcept : port_(port) {}

port::port(network_byte_order_t, std::uint16_t port) noexcept
    : port_(ntohs(port)) {}

auto port::to_bytes() const noexcept -> std::uint16_t { return port_; }

auto port::to_bytes(network_byte_order_t) const noexcept -> std::uint16_t {
  return htons(port_);
}

auto port::to_string() const noexcept -> std::string {
  return std::to_string(port_);
}

}  // namespace skynet::inet
