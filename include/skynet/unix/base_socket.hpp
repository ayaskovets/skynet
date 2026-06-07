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
  /**
   * @warning Bind for unix socket with SO_REUSEADDR enabled does smart unlink
   * in case address is already in use and retries bind. This is supported only
   * for this socket type specifically. For the base version use
   * skynet::base_socket
   */
  auto bind(skynet::unix::sockaddr const & sockaddr) -> skynet::bind_status;
  auto unlink_bind(skynet::unix::sockaddr const & sockaddr)
      -> skynet::bind_status;
  void unlink_close();
};

}  // namespace skynet::unix
