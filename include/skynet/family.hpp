#pragma once

#include <cstdint>
#include <format>

namespace skynet {

/**
 * @brief Address/protocol family
 */
enum class family : std::uint8_t {
  kUnspec,  ///< Not specified. Used in address resolution
  kInet,    ///< IPv4
  kInet6,   ///< IPv6
  kUnix,    ///< UNIX
};

auto to_native_family(family family) noexcept -> int;
auto from_native_family(int native_family) -> family;

}  // namespace skynet

template <>
class std::formatter<skynet::family> {
 public:
  template <typename FormatContext>
  constexpr auto parse(FormatContext & ctx) const noexcept {
    return ctx.begin();
  }

  template <typename FormatContext>
  constexpr auto format(skynet::family const & family,
                        FormatContext & ctx) const noexcept {
    switch (family) {
      case skynet::family::kUnspec:
        return std::format_to(ctx.out(), "AF_UNSPEC");
      case skynet::family::kInet:
        return std::format_to(ctx.out(), "AF_INET");
      case skynet::family::kInet6:
        return std::format_to(ctx.out(), "AF_INET6");
      case skynet::family::kUnix:
        return std::format_to(ctx.out(), "AF_UNIX");
    }
  }
};
