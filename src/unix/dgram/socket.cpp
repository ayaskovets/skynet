#include <skynet/unix/dgram/socket.hpp>

namespace skynet::unix::dgram {

socket::socket(uninitialized_t) noexcept
    : skynet::unix::base_socket(uninitialized_t{}) {}

socket::socket() : skynet::unix::base_socket(skynet::socktype::kDgram) {}

}  // namespace skynet::unix::dgram
