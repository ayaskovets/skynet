#include <skynet/inet6/udp/socket.hpp>

namespace skynet::inet6::udp {

socket::socket(uninitialized_t) noexcept
    : skynet::base_socket(uninitialized_t{}) {}

socket::socket()
    : skynet::base_socket(skynet::family::kInet6, skynet::socktype::kDgram,
                          skynet::protocol::kUdp) {}

}  // namespace skynet::inet6::udp
