#pragma once

#include "../base_sockaddr.hpp"
#include "ip.hpp"
#include "port.hpp"

namespace skynet::inet {

/**
 * @brief IPv4 address:port pair
 */
class sockaddr final : public base_sockaddr {
 public:
  // NOLINTNEXTLINE(readability-identifier-length)
  sockaddr(inet::ip ip, inet::port port);

 public:
  [[nodiscard]] auto ip() const noexcept -> inet::ip;
  [[nodiscard]] auto port() const noexcept -> inet::port;
};

}  // namespace skynet::inet

template <>
class std::formatter<skynet::inet::sockaddr> {
 public:
  template <typename FormatContext>
  constexpr auto parse(FormatContext & ctx) const noexcept {
    return ctx.begin();
  }

  template <typename FormatContext>
  constexpr auto format(skynet::inet::sockaddr const & sockaddr,
                        FormatContext & ctx) const {
    return std::format_to(ctx.out(), "{}", sockaddr.to_string());
  }
};
