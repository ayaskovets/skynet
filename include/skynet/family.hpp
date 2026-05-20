#pragma once

#include <cstdint>
#include <format>

namespace skynet {

/**
 * @brief Address family
 */
enum class family : std::uint8_t {
  kUnspecified,
  kInet,
  kInet6,
  kUnix,
};

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
      case skynet::family::kUnspecified:
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
