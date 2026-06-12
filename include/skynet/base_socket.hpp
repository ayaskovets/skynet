#pragma once

#include <span>

#include <swiss/static_pimpl.hpp>

#include "base_sockaddr.hpp"
#include "family.hpp"
#include "options.hpp"
#include "platform/platform.hpp"  // IWYU pragma: keep
#include "protocol.hpp"
#include "socktype.hpp"
#include "status.hpp"
#include "uninitialized.hpp"

namespace skynet {

/**
 * @brief Socket base class. Non-constructible by itself
 */
class base_socket {
  /**
   * @note Required for poller to access native_handle()
   */
  friend class poller;

 protected:
  explicit base_socket(uninitialized_t) noexcept;
  base_socket(family family, socktype type, protocol protocol);

 public:
  base_socket(base_socket const & that);
  auto operator=(base_socket const & that) -> base_socket &;
  base_socket(base_socket && that) noexcept;
  auto operator=(base_socket && that) noexcept -> base_socket &;
  ~base_socket() noexcept;

 protected:
  class impl;
  [[nodiscard]] auto native_handle() const noexcept -> impl const &;
  [[nodiscard]] auto native_handle() noexcept -> impl &;

 public:
  /**
   * @brief Manually close a socket
   *
   * This method explicitly throws an exception in case of any failure. This is
   * done to keep RAII semantics for the type but to allow user code to recv
   * the error if needed
   *
   * Any subsequent calls will result in an exception
   */
  void close();

 public:
  /**
   * @brief Identity comparison
   *
   * Socket is only equal to itself i.e. operator== is identity comparison
   *
   * An uninitialized or moved-from descriptor is equal to any other
   * descriptor in unitialized or moved-from state
   */
  auto operator==(base_socket const & that) const noexcept -> bool;
  auto operator!=(base_socket const & that) const noexcept -> bool;

 public:
  [[nodiscard]] auto family() const -> family;
  [[nodiscard]] auto socktype() const -> socktype;

 protected:
  void set_option(broadcast option);
  void set_option(keepalive option);
  void set_option(linger option);
  void set_option(nonblock option);
  void set_option(oobinline option);
  void set_option(rcvbuf option);
  void set_option(reuseaddr option);
  void set_option(reuseport option);
  void set_option(sndbuf option);
  void set_option(nodelay option);

 protected:
  void get_option(broadcast & option) const;
  void get_option(keepalive & option) const;
  void get_option(linger & option) const;
  void get_option(nonblock & option) const;
  void get_option(oobinline & option) const;
  void get_option(rcvbuf & option) const;
  void get_option(reuseaddr & option) const;
  void get_option(reuseport & option) const;
  void get_option(sndbuf & option) const;
  void get_option(nodelay & option) const;

 public:
  auto bind(base_sockaddr const & sockaddr) -> bind_status;
  auto connect(base_sockaddr const & sockaddr) -> connect_status;

 public:
  void getsockname(base_sockaddr & sockaddr) const;
  void getpeername(base_sockaddr & sockaddr) const;

 protected:
  void listen(std::size_t backlog);
  auto accept(base_socket & socket) const -> accept_status;

 protected:
  auto send(std::span<std::byte const> bytes) -> std::size_t;
  auto recv(std::span<std::byte> bytes) -> std::size_t;

 protected:
  auto sendto(std::span<std::byte const> bytes, base_sockaddr const & sockaddr)
      -> std::size_t;
  auto recvfrom(std::span<std::byte> bytes, base_sockaddr & sockaddr)
      -> std::size_t;

 private:
  swiss::static_pimpl<impl, platform::kNativeHandleSize,
                      platform::kNativeHandleAlignment>
      pimpl_;
};

}  // namespace skynet
