#include <skynet/inet/ip.hpp>

#include "platform/platform.hpp"  // IWYU pragma: keep

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
  if (string.size() > INET_ADDRSTRLEN) [[unlikely]] {
    throw std::invalid_argument("ip::ip(): invalid address");
  }

  // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage)
  switch (::inet_pton(AF_INET, string.data(), &data_)) {
    case 1:
      break;
    case 0:
      throw std::invalid_argument("ip::ip(): invalid address");
    default:
      throw std::invalid_argument(std::format("ip::ip(): inet_pton error {}",
                                              platform::errno_strerror()));
  }
}

auto ip::to_bytes() const noexcept -> std::uint32_t { return ntohl(data_); }

auto ip::to_bytes(network_byte_order_t) const noexcept -> std::uint32_t {
  return data_;
}

auto ip::to_string() const -> std::string {
  std::string string;
  string.resize(INET_ADDRSTRLEN);

  if (::inet_ntop(AF_INET, &data_, string.data(),
                  static_cast<platform::socklen_t>(string.size())) == nullptr)
      [[unlikely]] {
    throw std::runtime_error(std::format("ip::to_string(): inet_ntop error {}",
                                         platform::errno_strerror()));
  }

  string.resize(std::min(string.capacity(), string.find('\0')));
  return string;
}

// NOLINTEND(readability-identifier-length)
}  // namespace skynet::inet
