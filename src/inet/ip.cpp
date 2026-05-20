#include <skynet/inet/ip.hpp>

#include <cerrno>
#include <format>
#include <system_error>

#include <arpa/inet.h>

namespace skynet::inet {
// NOLINTBEGIN(readability-identifier-length)

auto ip::kLoopback() noexcept -> ip const & {
  static ip const ip(INADDR_LOOPBACK);
  return ip;
}

auto ip::kAny() noexcept -> ip const & {
  static ip const ip(INADDR_ANY);
  return ip;
}

auto ip::kBroadcast() noexcept -> ip const & {
  static ip const ip(INADDR_BROADCAST);
  return ip;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
auto ip::kNonRoutable() noexcept -> ip const & {
  /**
   * @note TEST-NET-1 address
   */
  static ip const ip("192.0.2.0");
  return ip;
}

ip::ip(std::uint32_t bytes) noexcept : data_(htonl(bytes)) {}

ip::ip(network_byte_order_t, std::uint32_t bytes) noexcept : data_(bytes) {}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
ip::ip(std::string_view string) {
  constexpr int kInetPtonSuccess = 1;

  if (string.size() > INET_ADDRSTRLEN) {
    throw std::invalid_argument(std::format("invalid ip: {}", string));
  }

  // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage)
  if (::inet_pton(AF_INET, string.data(), &data_) != kInetPtonSuccess)
      [[unlikely]] {
    throw std::invalid_argument(
        std::format("invalid ip {}: {}", string,
                    std::error_code(errno, std::generic_category()).message()));
  }
}

auto ip::to_bytes() const noexcept -> std::uint32_t { return ntohl(data_); }

auto ip::to_bytes(network_byte_order_t) const noexcept -> std::uint32_t {
  return data_;
}

auto ip::to_string() const -> std::string {
  constexpr auto kInetNtopError = nullptr;

  std::string string;
  string.resize(INET_ADDRSTRLEN);

  if (::inet_ntop(AF_INET, &data_, string.data(),
                  static_cast<::socklen_t>(string.size())) != kInetNtopError)
      [[likely]] {
    string.resize(string.find('\0'));
    return string;
  }

  throw std::runtime_error(
      std::format("could not serialize ip: {}",
                  std::error_code(errno, std::generic_category()).message()));
}

// NOLINTEND(readability-identifier-length)
}  // namespace skynet::inet
