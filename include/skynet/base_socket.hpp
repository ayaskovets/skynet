#pragma once

#include <span>

#include <swiss/static_pimpl.hpp>

#include "base_sockaddr.hpp"
#include "family.hpp"
#include "option.hpp"
#include "platform/platform.hpp"  // IWYU pragma: keep
#include "protocol.hpp"
#include "socktype.hpp"
#include "status.hpp"
#include "uninitialized.hpp"

namespace skynet {

/**
 * @brief Socket base class. Constructible only by derived types
 */
class base_socket {
 protected:
  /**
   * @brief Create an unitialized handle without any system calls
   */
  explicit base_socket(uninitialized_t) noexcept;

  /**
   * @brief Allocate a socket handle
   */
  base_socket(family family, socktype type, protocol protocol);

 public:
  base_socket(base_socket const & that);
  auto operator=(base_socket const & that) -> base_socket &;
  base_socket(base_socket && that) noexcept;
  auto operator=(base_socket && that) noexcept -> base_socket &;
  ~base_socket() noexcept;

 public:
  /**
   * @brief Identity comparison
   * @note Socket is only equal to itself i.e. operator== is identity comparison
   * @note An uninitialized or moved-from descriptor is equal to any other
   * descriptor in unitialized or moved-from state
   */
  auto operator==(base_socket const & that) const noexcept -> bool;

 public:
  [[nodiscard]] auto family() const -> family;
  [[nodiscard]] auto socktype() const -> socktype;
  [[nodiscard]] auto hash() const noexcept -> std::size_t;

  auto bind(base_sockaddr const & sockaddr) -> bind_status;
  auto connect(base_sockaddr const & sockaddr) -> connect_status;

  void getsockname(base_sockaddr & sockaddr) const;
  void getpeername(base_sockaddr & sockaddr) const;

  /**
   * @brief Manually close a socket
   * @note This method explicitly throws an exception in case of any failure.
   * This is done to keep RAII semantics for the type but to allow user code to
   * recv the error if needed
   * @note Any subsequent calls will result in an exception
   */
  void close();

 protected:
  void set_option(concepts::option auto option);
  void get_option(concepts::option auto & option) const;

  void listen(std::size_t backlog);
  auto accept(base_socket & socket) const -> accept_status;

  auto send(std::span<std::byte const> bytes) -> std::size_t;
  auto recv(std::span<std::byte> bytes) -> std::size_t;

  auto sendto(std::span<std::byte const> bytes, base_sockaddr const & sockaddr)
      -> std::size_t;
  auto recvfrom(std::span<std::byte> bytes, base_sockaddr & sockaddr)
      -> std::size_t;

 protected:
  class impl;
  [[nodiscard]] auto native_handle() const noexcept -> impl const &;
  [[nodiscard]] auto native_handle() noexcept -> impl &;

 private:
  swiss::static_pimpl<impl, platform::kNativeHandleSize,
                      platform::kNativeHandleAlignment>
      pimpl_;

 private:
  /**
   * @note Required for poller to access native_handle()
   */
  friend class poller;
};

}  // namespace skynet

/**
 * @brief Hash specialization for base_socket
 */
template <>
struct std::hash<skynet::base_socket> {
  auto operator()(skynet::base_socket const & socket) const noexcept
      -> std::size_t;
};
