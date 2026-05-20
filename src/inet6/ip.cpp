#include <skynet/inet6/ip.hpp>

#include <cerrno>
#include <format>
#include <system_error>

#include <arpa/inet.h>

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
  constexpr int kInetPtonSuccess = 1;

  if (string.size() > INET6_ADDRSTRLEN) {
    throw std::invalid_argument(std::format("invalid ip: {}", string));
  }

  // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage)
  if (::inet_pton(AF_INET6, string.data(), &data_) != kInetPtonSuccess) {
    throw std::invalid_argument(
        std::format("invalid ip {}: {}", string,
                    std::error_code(errno, std::generic_category()).message()));
  }
}

auto ip::to_bytes(network_byte_order_t) const noexcept
    -> std::array<std::byte, 16> {
  return data_;
}

auto ip::to_string() const -> std::string {
  constexpr auto kInetNtopError = nullptr;

  std::string string;
  string.resize(INET6_ADDRSTRLEN);

  // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage)
  if (::inet_ntop(AF_INET6, &data_, string.data(),
                  static_cast<::socklen_t>(string.size())) != kInetNtopError)
      [[likely]] {
    string.resize(string.find('\0'));
    return string;
  }

  throw std::runtime_error(
      std::format("could not serialize ip: {}",
                  std::error_code(errno, std::generic_category()).message()));
}

// NOLINTEND(readability-identifier-length,bugprone-exception-escape)
}  // namespace skynet::inet6
