#include <skynet/inet/udp/socket.hpp>

namespace skynet::inet::udp {

socket::socket(uninitialized_t) noexcept : skynet::base_socket(uninitialized) {}

socket::socket()
    : skynet::base_socket(skynet::family::kInet, skynet::socktype::kDgram,
                          skynet::protocol::kUdp) {}

}  // namespace skynet::inet::udp
