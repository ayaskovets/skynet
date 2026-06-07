#pragma once

#include <skynet/base_socket.hpp>
#include <skynet/unix/sockaddr.hpp>

namespace skynet::unix {

/**
 * @brief UNIX socket base class
 */
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class base_socket : public skynet::base_socket {
 public:
  explicit base_socket(uninitialized_t) noexcept;
  explicit base_socket(skynet::socktype type);
  ~base_socket() noexcept;

 public:
  using skynet::base_socket::get_option;
  using skynet::base_socket::recvfrom;
  using skynet::base_socket::sendto;
  using skynet::base_socket::set_option;

 public:
  auto unlink_bind(skynet::unix::sockaddr const & sockaddr) noexcept(false)
      -> skynet::bind_status;
  void unlink_close();
};

}  // namespace skynet::unix
