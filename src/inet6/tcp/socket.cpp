#include <skynet/inet6/tcp/socket.hpp>

namespace skynet::inet6::tcp {

socket::socket(uninitialized_t) noexcept
    : skynet::base_socket(uninitialized_t{}) {}

socket::socket()
    : skynet::base_socket(skynet::family::kInet6, skynet::socktype::kStream,
                          skynet::protocol::kTcp) {}

}  // namespace skynet::inet6::tcp
