#pragma once

#include <cstdint>
#include <format>

namespace skynet {

/**
 * @brief Protocol type
 */
enum class socktype : std::uint8_t {
  kStream,
  kDgram,
  kRaw,
  kRdm,
  kSeqpacket,
};

}  // namespace skynet

template <>
class std::formatter<skynet::socktype> {
 public:
  template <typename FormatContext>
  constexpr auto parse(FormatContext & ctx) const noexcept {
    return ctx.begin();
  }

  template <typename FormatContext>
  constexpr auto format(skynet::socktype const & type,
                        FormatContext & ctx) const noexcept {
    switch (type) {
      case skynet::socktype::kStream:
        return std::format_to(ctx.out(), "SOCK_STREAM");
      case skynet::socktype::kDgram:
        return std::format_to(ctx.out(), "SOCK_DGRAM");
      case skynet::socktype::kRaw:
        return std::format_to(ctx.out(), "SOCK_RAW");
      case skynet::socktype::kRdm:
        return std::format_to(ctx.out(), "SOCK_RDM");
      case skynet::socktype::kSeqpacket:
        return std::format_to(ctx.out(), "SOCK_SEQPACKET");
    }
  }
};
