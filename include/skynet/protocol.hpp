#pragma once

#include <cstdint>
#include <format>

namespace skynet {

/**
 * @brief Specific protocol
 */
enum class protocol : std::uint8_t {
  kDefault,
  kTcp,
  kUdp,
};

auto to_native_protocol(protocol protocol) noexcept -> int;
auto from_native_protocol(int native_protocol) -> protocol;

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
      case skynet::protocol::kDefault:
        return std::format_to(ctx.out(), "DEFAULT");
      case skynet::protocol::kTcp:
        return std::format_to(ctx.out(), "IPPROTO_TCP");
      case skynet::protocol::kUdp:
        return std::format_to(ctx.out(), "IPPROTO_UDP");
    }
  }
};
