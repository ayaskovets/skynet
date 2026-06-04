#include <skynet/inet/tcp/socket.hpp>

namespace skynet::inet::tcp {

socket::socket(uninitialized_t) noexcept
    : skynet::base_socket(uninitialized_t{}) {}

socket::socket()
    : skynet::base_socket(skynet::family::kInet, skynet::socktype::kStream,
                          skynet::protocol::kTcp) {}

}  // namespace skynet::inet::tcp
