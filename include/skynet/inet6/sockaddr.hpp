#pragma once

#include "../base_sockaddr.hpp"
#include "ip.hpp"
#include "port.hpp"

namespace skynet::inet6 {

/**
 * @brief IPv6 address:port pair
 */
class sockaddr final : public base_sockaddr {
 public:
  // NOLINTNEXTLINE(readability-identifier-length)
  sockaddr(inet6::ip ip, inet6::port port);

 public:
  [[nodiscard]] auto ip() const noexcept -> inet6::ip;
  [[nodiscard]] auto port() const noexcept -> inet6::port;
};

}  // namespace skynet::inet6

template <>
class std::formatter<skynet::inet6::sockaddr> {
 public:
  template <typename FormatContext>
  constexpr auto parse(FormatContext & ctx) const noexcept {
    return ctx.begin();
  }

  template <typename FormatContext>
  constexpr auto format(skynet::inet6::sockaddr const & sockaddr,
                        FormatContext & ctx) const {
    return std::format_to(ctx.out(), "{}", sockaddr.to_string());
  }
};
