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
 * @brief Socket base class
 */
class base_socket {
 protected:
  explicit base_socket(uninitialized_t) noexcept;
  base_socket(family family, socktype type, protocol protocol);

 public:
  base_socket(base_socket const & that);
  auto operator=(base_socket const & that) -> base_socket &;
  base_socket(base_socket && that) noexcept;
  // NOLINTNEXTLINE(cppcoreguidelines-noexcept-move-operations)
  auto operator=(base_socket && that) -> base_socket &;
  ~base_socket() noexcept;

 public:
  class impl;
  [[nodiscard]] auto native_handle() const noexcept -> impl const &;

 public:
  /**
   * @brief Manually close a socket
   *
   * This method explicitly throws an exception in case of any failure. This is
   * done to keep RAII semantics for the type but to allow user code to receive
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
  [[nodiscard]] auto type() const -> socktype;
  [[nodiscard]] auto protocol() const -> protocol;

 public:
  void set_option(acceptconn option);
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

 public:
  [[nodiscard]] auto get_option(acceptconn_t) const -> acceptconn;
  [[nodiscard]] auto get_option(broadcast_t) const -> broadcast;
  [[nodiscard]] auto get_option(keepalive_t) const -> keepalive;
  [[nodiscard]] auto get_option(linger_t) const -> linger;
  [[nodiscard]] auto get_option(nonblock_t) const -> nonblock;
  [[nodiscard]] auto get_option(oobinline_t) const -> oobinline;
  [[nodiscard]] auto get_option(rcvbuf_t) const -> rcvbuf;
  [[nodiscard]] auto get_option(reuseaddr_t) const -> reuseaddr;
  [[nodiscard]] auto get_option(reuseport_t) const -> reuseport;
  [[nodiscard]] auto get_option(sndbuf_t) const -> sndbuf;
  [[nodiscard]] auto get_option(nodelay_t) const -> nodelay;

 public:
  auto bind(base_sockaddr const & sockaddr) -> bind_status;
  auto connect(base_sockaddr const & sockaddr) -> connect_status;

 public:
  void bind_sockaddr(base_sockaddr & sockaddr) const;
  void connect_sockaddr(base_sockaddr & sockaddr) const;

 public:
  void listen(std::size_t backlog);
  auto accept(base_socket & socket) const -> accept_status;

 public:
  auto send(std::span<std::byte const> bytes) -> std::size_t;
  auto receive(std::span<std::byte> bytes) -> std::size_t;

 public:
  auto send_to(std::span<std::byte const> bytes, base_sockaddr const & sockaddr)
      -> std::size_t;
  auto receive_from(std::span<std::byte> bytes, base_sockaddr & sockaddr)
      -> std::size_t;

 private:
  [[nodiscard]] auto native_handle() noexcept -> impl &;

 private:
  swiss::static_pimpl<impl, platform::kNativeHandleSize,
                      platform::kNativeHandleAlignment>
      pimpl_;
};

}  // namespace skynet
