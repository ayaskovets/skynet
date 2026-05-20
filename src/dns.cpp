#include <skynet/dns.hpp>

#include <cerrno>
#include <format>
#include <span>
#include <system_error>

#include <netdb.h>

namespace skynet {

namespace {

constexpr auto to_native_family(family family) -> int {
  switch (family) {
    case family::kUnspecified:
      return AF_UNSPEC;
    case family::kInet:
      return AF_INET;
    case family::kInet6:
      return AF_INET6;
    case family::kUnix:
      throw std::invalid_argument(
          std::format("unsupported family: {}", family));
  }
}

constexpr auto to_native_type(protocol protocol) noexcept -> int {
  switch (protocol) {
    case protocol::kUnspecified:
      return 0;
    case protocol::kTcp:
      return SOCK_STREAM;
    case protocol::kUdp:
      return SOCK_DGRAM;
  }
}

constexpr auto to_sockaddr(::addrinfo const & addrinfo)
    -> std::variant<inet::sockaddr, inet6::sockaddr> {
  switch (addrinfo.ai_family) {
    case AF_INET: {
      auto const * sockaddr = static_cast<::sockaddr_in const *>(
          // NOLINTNEXTLINE(bugprone-casting-through-void)
          static_cast<void const *>(addrinfo.ai_addr));

      return inet::sockaddr(
          inet::ip(network_byte_order_t{}, sockaddr->sin_addr.s_addr),
          inet::port(network_byte_order_t{}, sockaddr->sin_port));
    }
    case AF_INET6: {
      auto const * sockaddr = static_cast<::sockaddr_in6 const *>(
          // NOLINTNEXTLINE(bugprone-casting-through-void)
          static_cast<void const *>(addrinfo.ai_addr));

      return inet6::sockaddr(
          inet6::ip(
              network_byte_order_t{},
              std::bit_cast<std::array<std::byte, sizeof(sockaddr->sin6_addr)>>(
                  sockaddr->sin6_addr)),
          inet6::port(network_byte_order_t{}, sockaddr->sin6_port));
    }
    default:
      throw std::runtime_error(
          std::format("unexpected address family {}", addrinfo.ai_family));
  }
}

}  // namespace

auto resolve(std::string_view hostname, family family, protocol protocol)
    -> std::vector<std::variant<inet::sockaddr, inet6::sockaddr>> {
  std::unique_ptr<::addrinfo, decltype(&::freeaddrinfo)> results_raii(
      nullptr, ::freeaddrinfo);

  ::addrinfo const hints{
      .ai_family   = to_native_family(family),
      .ai_socktype = to_native_type(protocol),
  };

  ::addrinfo * results = nullptr;

  // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage)
  auto const retval = ::getaddrinfo(hostname.data(), nullptr, &hints, &results);
  switch (retval) {
    case 0:
      results_raii.reset(results);
      break;
    [[unlikely]] case EAI_SYSTEM:
      throw std::runtime_error(std::format(
          "failed to resolve any address at {}: {}", hostname,
          std::error_code(errno, std::generic_category()).message()));
    default:
      throw std::runtime_error(
          std::format("failed to resolve any address at {}: {}", hostname,
                      ::gai_strerror(retval)));
  }

  std::vector<std::variant<inet::sockaddr, inet6::sockaddr>> sockaddrs;
  for (::addrinfo const * ptr = results; ptr != nullptr; ptr = ptr->ai_next) {
    sockaddrs.emplace_back(to_sockaddr(*ptr));
  }
  return sockaddrs;
}

}  // namespace skynet
