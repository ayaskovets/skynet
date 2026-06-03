#include <skynet/inet6/ip.hpp>

#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet::inet6 {
// NOLINTBEGIN(readability-identifier-length,bugprone-exception-escape)

auto ip::kLoopback() noexcept -> ip const & {
  static ip const ip("::1");
  return ip;
}

auto ip::kAny() noexcept -> ip const & {
  static ip const ip("::");
  return ip;
}

auto ip::kNonRoutable() noexcept -> ip const & {
  /**
   * @note Reserved for documentation
   */
  static ip const ip("2001:db8::");
  return ip;
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
ip::ip(network_byte_order_t, std::span<std::byte const, 16> bytes) noexcept {
  std::memcpy(data_.data(), bytes.data(), bytes.size());
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
ip::ip(std::string_view string) {
  if (string.size() > INET6_ADDRSTRLEN) {
    throw std::invalid_argument(std::format("invalid ip: {}", string));
  }

  // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage)
  if (::inet_pton(AF_INET6, string.data(), &data_) != 1) {
    throw std::invalid_argument(
        std::format("invalid ip {}: {}", string, platform::errno_strerror()));
  }
}

auto ip::to_bytes(network_byte_order_t) const noexcept
    -> std::array<std::byte, 16> {
  return data_;
}

auto ip::to_string() const -> std::string {
  std::string string;
  string.resize(INET6_ADDRSTRLEN);

  // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage)
  if (::inet_ntop(AF_INET6, &data_, string.data(),
                  static_cast<::socklen_t>(string.size())) == nullptr) {
    throw std::runtime_error(
        std::format("could not serialize ip: {}", platform::errno_strerror()));
  }

  string.resize(std::min(string.capacity(), string.find('\0')));
  return string;
}

// NOLINTEND(readability-identifier-length,bugprone-exception-escape)
}  // namespace skynet::inet6
