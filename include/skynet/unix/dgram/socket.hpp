#pragma once

#include <skynet/unix/base_socket.hpp>

namespace skynet::unix::dgram {

/**
 * @brief UNIX datagram socket
 */
class socket final : public skynet::unix::base_socket {
 public:
  explicit socket(uninitialized_t) noexcept;
  socket();

 public:
  using skynet::base_socket::get_option;
  using skynet::base_socket::recv;
  using skynet::base_socket::recvfrom;
  using skynet::base_socket::send;
  using skynet::base_socket::sendto;
  using skynet::base_socket::set_option;
};

}  // namespace skynet::unix::dgram
