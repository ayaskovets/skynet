#include <skynet/unix/stream/socket.hpp>

namespace skynet::unix::stream {

socket::socket(uninitialized_t) noexcept
    : skynet::unix::base_socket(uninitialized) {}

socket::socket() : skynet::unix::base_socket(skynet::socktype::kStream) {}

}  // namespace skynet::unix::stream
