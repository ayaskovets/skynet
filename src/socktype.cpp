#include <skynet/socktype.hpp>

#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet {

auto to_native_socktype(socktype socktype) noexcept -> int {
  switch (socktype) {
    case socktype::kUnspec:
      return 0;
    case socktype::kStream:
      return SOCK_STREAM;
    case socktype::kDgram:
      return SOCK_DGRAM;
    case socktype::kRaw:
      return SOCK_RAW;
    case socktype::kRdm:
      return SOCK_RDM;
    case socktype::kSeqpacket:
      return SOCK_SEQPACKET;
  }
}

auto from_native_socktype(int native_socktype) -> socktype {
  switch (native_socktype) {
    case 0:
      return socktype::kUnspec;
    case SOCK_STREAM:
      return socktype::kStream;
    case SOCK_DGRAM:
      return socktype::kDgram;
    case SOCK_RAW:
      return socktype::kRaw;
    case SOCK_RDM:
      return socktype::kRdm;
    case SOCK_SEQPACKET:
      return socktype::kSeqpacket;
    [[unlikely]] default:
      throw std::invalid_argument(
          "from_native_socktype(): invalid native socktype");
  }
}

}  // namespace skynet
