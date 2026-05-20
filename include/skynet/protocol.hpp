#pragma once

#include <cstdint>
#include <format>

namespace skynet {

/**
 * @brief Specific protocol
 */
enum class protocol : std::uint8_t {
  kUnspecified,
  kTcp,
  kUdp,
};

}  // namespace skynet

template <>
class std::formatter<skynet::protocol> {
 public:
  template <typename FormatContext>
  constexpr auto parse(FormatContext & ctx) const noexcept {
    return ctx.begin();
  }

  template <typename FormatContext>
  constexpr auto format(skynet::protocol const & protocol,
                        FormatContext & ctx) const noexcept {
    switch (protocol) {
      case skynet::protocol::kUnspecified:
        return std::format_to(ctx.out(), "UNSPECIFIED");
      case skynet::protocol::kTcp:
        return std::format_to(ctx.out(), "IPPROTO_TCP");
      case skynet::protocol::kUdp:
        return std::format_to(ctx.out(), "IPPROTO_UDP");
    }
  }
};
