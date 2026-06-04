#pragma once

#include <skynet/base_socket.hpp>

namespace skynet::inet::tcp {

/**
 * @brief IPv4 TCP socket
 */
class socket final : public skynet::base_socket {
 public:
  explicit socket(uninitialized_t) noexcept;
  socket();

 public:
  using skynet::base_socket::accept;
  using skynet::base_socket::get_option;
  using skynet::base_socket::listen;
  using skynet::base_socket::recv;
  using skynet::base_socket::send;
  using skynet::base_socket::set_option;
};

}  // namespace skynet::inet::tcp
