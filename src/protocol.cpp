#include <skynet/protocol.hpp>

#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet {

auto to_native_protocol(protocol protocol) noexcept -> int {
  switch (protocol) {
    case protocol::kIp:
      return IPPROTO_IP;
    case protocol::kTcp:
      return IPPROTO_TCP;
    case protocol::kUdp:
      return IPPROTO_UDP;
  }
}

auto from_native_protocol(int native_protocol) -> protocol {
  switch (native_protocol) {
    case IPPROTO_IP:
      return protocol::kIp;
    case IPPROTO_TCP:
      return protocol::kTcp;
    case IPPROTO_UDP:
      return protocol::kUdp;
    [[unlikely]] default:
      throw std::invalid_argument(std::format(
          "invalid/unimplemented native protocol {}", native_protocol));
  }
}

}  // namespace skynet
