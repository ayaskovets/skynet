#pragma once

#include <cstdint>
#include <format>

#include "../network_byte_order.hpp"

namespace skynet::inet {

/**
 * @brief IPv4 port
 */
class port final {
 public:
  explicit port(std::uint16_t port) noexcept;
  port(network_byte_order_t, std::uint16_t port) noexcept;

 public:
  constexpr auto operator==(port const &) const noexcept -> bool = default;
  constexpr auto operator!=(port const &) const noexcept -> bool = default;

 public:
  [[nodiscard]] auto to_bytes() const noexcept -> std::uint16_t;
  [[nodiscard]] auto to_bytes(network_byte_order_t) const noexcept
      -> std::uint16_t;
  [[nodiscard]] auto to_string() const noexcept -> std::string;

 private:
  std::uint16_t port_;
};

}  // namespace skynet::inet

template <>
class std::formatter<skynet::inet::port> {
 public:
  template <typename FormatContext>
  constexpr auto parse(FormatContext & ctx) const noexcept {
    return ctx.begin();
  }

  template <typename FormatContext>
  constexpr auto format(skynet::inet::port const & port,
                        FormatContext & ctx) const noexcept {
    return std::format_to(ctx.out(), "{}", port.to_string());
  }
};
