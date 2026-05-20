#pragma once

#include <format>
#include <string>

#include "../network_byte_order.hpp"

namespace skynet::inet {

/**
 * @brief IPv4 address
 */
class ip final {
 public:
  static auto kLoopback() noexcept -> ip const &;
  static auto kNonRoutable() noexcept -> ip const &;
  static auto kAny() noexcept -> ip const &;
  static auto kBroadcast() noexcept -> ip const &;

 public:
  explicit ip(std::uint32_t bytes) noexcept;
  ip(network_byte_order_t, std::uint32_t bytes) noexcept;
  explicit ip(std::string_view string);

 public:
  constexpr auto operator==(ip const &) const noexcept -> bool = default;
  constexpr auto operator!=(ip const &) const noexcept -> bool = default;

 public:
  [[nodiscard]] auto to_bytes() const noexcept -> std::uint32_t;
  [[nodiscard]] auto to_bytes(network_byte_order_t) const noexcept
      -> std::uint32_t;
  [[nodiscard]] auto to_string() const -> std::string;

 private:
  std::uint32_t data_;
};

}  // namespace skynet::inet

template <>
class std::formatter<skynet::inet::ip> {
 public:
  template <typename FormatContext>
  constexpr auto parse(FormatContext & ctx) const noexcept {
    return ctx.begin();
  }

  template <typename FormatContext>
  // NOLINTNEXTLINE(readability-identifier-length)
  constexpr auto format(skynet::inet::ip const & ip,
                        FormatContext & ctx) const {
    return std::format_to(ctx.out(), "{}", ip.to_string());
  }
};
