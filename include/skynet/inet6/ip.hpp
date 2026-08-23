#pragma once

#include <format>
#include <span>
#include <string>

#include "../network_byte_order.hpp"

namespace skynet::inet6 {

/**
 * @brief IPv6 address
 */
class ip final {
 private:
  static constexpr std::size_t kIpV6Bytes = 16;

 public:
  static auto kLoopback() noexcept -> ip const &;
  static auto kNonRoutable() noexcept -> ip const &;
  static auto kAny() noexcept -> ip const &;

 public:
  ip(network_byte_order_t,
     std::span<std::byte const, kIpV6Bytes> bytes) noexcept;
  explicit ip(std::string_view string);

 public:
  constexpr auto operator==(ip const &) const noexcept -> bool = default;

 public:
  [[nodiscard]] auto to_bytes(network_byte_order_t) const noexcept
      -> std::array<std::byte, kIpV6Bytes>;
  [[nodiscard]] auto to_string() const -> std::string;

 private:
  std::array<std::byte, kIpV6Bytes> data_;
};

}  // namespace skynet::inet6

template <>
class std::formatter<skynet::inet6::ip> {
 public:
  template <typename FormatContext>
  constexpr auto parse(FormatContext & ctx) const noexcept {
    return ctx.begin();
  }

  template <typename FormatContext>
  // NOLINTNEXTLINE(readability-identifier-length)
  constexpr auto format(skynet::inet6::ip const & ip,
                        FormatContext & ctx) const {
    return std::format_to(ctx.out(), "{}", ip.to_string());
  }
};
