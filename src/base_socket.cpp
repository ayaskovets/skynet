#include <skynet/base_socket.hpp>
#include <skynet/options.hpp>

#include <chrono>

#include <swiss/strong_typedef.hpp>

#include "base_socket_impl.hpp"
#include "platform/platform.hpp"  // IWYU pragma: keep
#include "platform/unix.hpp"

namespace skynet {

base_socket::base_socket(uninitialized_t) noexcept
    : pimpl_(impl(platform::kInvalidNativeHandle)) {}

base_socket::base_socket(enum family family, enum socktype socktype,
                         enum protocol protocol)
    : pimpl_([family, socktype, protocol] -> impl {
        platform::native_handle_t const handle = platform::create_native_handle(
            to_native_family(family), to_native_socktype(socktype),
            to_native_protocol(protocol));

        if (handle == platform::kInvalidNativeHandle) [[unlikely]] {
          throw std::runtime_error(
              std::format("socket failed: {}", platform::errno_strerror()));
        }

        // NOLINTNEXTLINE(modernize-return-braced-init-list)
        return impl(handle);
      }()) {}

base_socket::base_socket(base_socket const & that)
    : base_socket(uninitialized_t{}) {
  if (*that.native_handle() == platform::kInvalidNativeHandle) [[unlikely]] {
    return;
  }

  platform::native_handle_t const handle =
      platform::copy_native_handle(*that.native_handle());
  if (handle == platform::kInvalidNativeHandle) [[unlikely]] {
    throw std::runtime_error(
        std::format("failed to clone socket: {}", platform::errno_strerror()));
  }

  *pimpl_ = impl(handle);
}

auto base_socket::operator=(base_socket const & that) -> base_socket & {
  if (this == &that) [[unlikely]] {
    return *this;
  }

  platform::native_handle_t const handle =
      platform::copy_native_handle(*that.native_handle());
  if (handle == platform::kInvalidNativeHandle) [[unlikely]] {
    throw std::runtime_error(
        std::format("failed to clone socket: {} ", platform::errno_strerror()));
  }

  if (*native_handle() != platform::kInvalidNativeHandle) {
    std::ignore = platform::close_native_handle(*native_handle());
  }

  *pimpl_ = impl(handle);
  return *this;
}

base_socket::base_socket(base_socket && that) noexcept
    : pimpl_(
          std::exchange(*that.pimpl_, impl(platform::kInvalidNativeHandle))) {}

auto base_socket::operator=(base_socket && that) noexcept -> base_socket & {
  if (this == &that) [[unlikely]] {
    return *this;
  }

  if (*native_handle() != platform::kInvalidNativeHandle) {
    std::ignore = platform::close_native_handle(*native_handle());
  }

  pimpl_ = std::exchange(that.pimpl_, impl(platform::kInvalidNativeHandle));
  return *this;
}

base_socket::~base_socket() noexcept {
  if (*native_handle() == platform::kInvalidNativeHandle) [[unlikely]] {
    return;
  }

  std::ignore = platform::close_native_handle(*native_handle());
}

auto base_socket::native_handle() const noexcept -> impl const & {
  return *pimpl_;
}

auto base_socket::native_handle() noexcept -> impl & { return *pimpl_; }

void base_socket::close() {
  if (*native_handle() == platform::kInvalidNativeHandle) [[unlikely]] {
    throw std::runtime_error("close on a closed socket");
  }

  auto const retval = platform::close_native_handle(
      std::exchange(*native_handle(), platform::kInvalidNativeHandle));
  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("close failed: {} ", platform::errno_strerror()));
  }
}

auto base_socket::operator==(base_socket const & that) const noexcept -> bool {
  return native_handle() == that.native_handle();
}

auto base_socket::operator!=(base_socket const & that) const noexcept -> bool {
  return !operator==(that);
}

auto base_socket::family() const -> skynet::family {
  ::sockaddr_storage storage{};
  platform::socklen_t socklen = sizeof(storage);
  auto const retval           = ::getsockname(
      *native_handle(),
      // NOLINTNEXTLINE(bugprone-casting-through-void)
      static_cast<::sockaddr *>(static_cast<void *>(&storage)), &socklen);

  if (retval == platform::kErrorRetval) {
    throw std::runtime_error(
        std::format("getsockname failed: {}", platform::errno_strerror()));
  }

  return from_native_family(storage.ss_family);
}

auto base_socket::socktype() const -> skynet::socktype {
  int optval{};
  platform::socklen_t optlen = sizeof(optval);
  auto const retval =
      ::getsockopt(*native_handle(), SOL_SOCKET, SO_TYPE, &optval, &optlen);

  if (retval == platform::kErrorRetval || optlen != sizeof(optval))
      [[unlikely]] {
    throw std::runtime_error(
        std::format("getsockopt failed: {} ", platform::errno_strerror()));
  }

  return from_native_socktype(optval);
}

void base_socket::set_option(broadcast option) {
  int const optval  = (*option ? 1 : 0);
  auto const retval = ::setsockopt(*native_handle(), SOL_SOCKET, SO_BROADCAST,
                                   &optval, sizeof(optval));

  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("set broadcast failed: {}", platform::errno_strerror()));
  }
}

void base_socket::set_option(keepalive option) {
  int const optval  = (*option ? 1 : 0);
  auto const retval = ::setsockopt(*native_handle(), SOL_SOCKET, SO_KEEPALIVE,
                                   &optval, sizeof(optval));

  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("set keepalive failed: {}", platform::errno_strerror()));
  }
}

void base_socket::set_option(linger option) {
  ::linger const optval{.l_onoff  = (option->has_value() ? 1 : 0),
                        .l_linger = static_cast<int>(
                            option->value_or(std::chrono::seconds{}).count())};
  auto const retval = ::setsockopt(*native_handle(), SOL_SOCKET, SO_LINGER_SEC,
                                   &optval, sizeof(optval));

  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("set nodelay failed: {}", platform::errno_strerror()));
  }
}

void base_socket::set_option(nodelay option) {
  int const optval  = (*option ? 1 : 0);
  auto const retval = ::setsockopt(*native_handle(), IPPROTO_TCP, TCP_NODELAY,
                                   &optval, sizeof(optval));

  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("set nodelay failed: {}", platform::errno_strerror()));
  }
}

void base_socket::set_option(nonblock option) {
  auto const retval = platform::set_nonblock(*native_handle(), *option ? 1 : 0);
  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("set nonblock failed: {}", platform::errno_strerror()));
  }
}

void base_socket::set_option(oobinline option) {
  int const optval  = (*option ? 1 : 0);
  auto const retval = ::setsockopt(*native_handle(), SOL_SOCKET, SO_OOBINLINE,
                                   &optval, sizeof(optval));

  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("set oobinline failed: {}", platform::errno_strerror()));
  }
}

void base_socket::set_option(rcvbuf option) {
  int const optval  = static_cast<int>(std::min(
      *option, static_cast<std::uint32_t>(std::numeric_limits<int>::max())));
  auto const retval = ::setsockopt(*native_handle(), SOL_SOCKET, SO_RCVBUF,
                                   &optval, sizeof(optval));

  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("set reuseport failed: {}", platform::errno_strerror()));
  }
}

void base_socket::set_option(reuseaddr option) {
  int const optval  = (*option ? 1 : 0);
  auto const retval = ::setsockopt(*native_handle(), SOL_SOCKET, SO_REUSEADDR,
                                   &optval, sizeof(optval));

  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("set reuseaddr failed: {}", platform::errno_strerror()));
  }
}

void base_socket::set_option(reuseport option) {
  int const optval  = (*option ? 1 : 0);
  auto const retval = ::setsockopt(*native_handle(), SOL_SOCKET, SO_REUSEPORT,
                                   &optval, sizeof(optval));

  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("set reuseport failed: {}", platform::errno_strerror()));
  }
}

void base_socket::set_option(sndbuf option) {
  int const optval  = static_cast<int>(std::min(
      *option, static_cast<std::uint32_t>(std::numeric_limits<int>::max())));
  auto const retval = ::setsockopt(*native_handle(), SOL_SOCKET, SO_SNDBUF,
                                   &optval, sizeof(optval));

  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("set sndbuf failed: {}", platform::errno_strerror()));
  }
}

void base_socket::get_option(broadcast & option) const {
  int optval{};
  platform::socklen_t optlen = sizeof(optval);
  auto const retval = ::getsockopt(*native_handle(), SOL_SOCKET, SO_BROADCAST,
                                   &optval, &optlen);

  if (retval == platform::kErrorRetval || optlen != sizeof(optval))
      [[unlikely]] {
    throw std::runtime_error(
        std::format("get broadcast failed: {}", platform::errno_strerror()));
  }

  *option = (optval != 0);
}

void base_socket::get_option(keepalive & option) const {
  int optval{};
  platform::socklen_t optlen = sizeof(optval);
  auto const retval = ::getsockopt(*native_handle(), SOL_SOCKET, SO_KEEPALIVE,
                                   &optval, &optlen);

  if (retval == platform::kErrorRetval || optlen != sizeof(optval))
      [[unlikely]] {
    throw std::runtime_error(
        std::format("get keepalive failed: {}", platform::errno_strerror()));
  }

  *option = (optval != 0);
}

void base_socket::get_option(linger & option) const {
  ::linger optval{};
  platform::socklen_t optlen = sizeof(optval);
  auto const retval = ::getsockopt(*native_handle(), SOL_SOCKET, SO_LINGER_SEC,
                                   &optval, &optlen);

  if (retval == platform::kErrorRetval || optlen != sizeof(optval))
      [[unlikely]] {
    throw std::runtime_error(
        std::format("get nodelay failed: {}", platform::errno_strerror()));
  }

  *option = (optval.l_onoff == 0 ? std::optional<std::chrono::seconds>()
                                 : std::chrono::seconds(optval.l_linger));
}

void base_socket::get_option(nodelay & option) const {
  int optval{};
  platform::socklen_t optlen = sizeof(optval);
  auto const retval = ::getsockopt(*native_handle(), IPPROTO_TCP, TCP_NODELAY,
                                   &optval, &optlen);

  if (retval == platform::kErrorRetval || optlen != sizeof(optval))
      [[unlikely]] {
    throw std::runtime_error(
        std::format("get nodelay failed: {}", platform::errno_strerror()));
  }

  *option = (optval != 0);
}

void base_socket::get_option(nonblock & option) const {
  auto const retval = platform::get_nonblock(*native_handle());
  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("get nonblock failed: {}", platform::errno_strerror()));
  }

  *option = (retval != 0);
}

void base_socket::get_option(oobinline & option) const {
  int optval{};
  platform::socklen_t optlen = sizeof(optval);
  auto const retval = ::getsockopt(*native_handle(), SOL_SOCKET, SO_OOBINLINE,
                                   &optval, &optlen);

  if (retval == platform::kErrorRetval || optlen != sizeof(optval))
      [[unlikely]] {
    throw std::runtime_error(
        std::format("get oobinline failed: {}", platform::errno_strerror()));
  }

  *option = (optval != 0);
}

void base_socket::get_option(rcvbuf & option) const {
  int optval{};
  platform::socklen_t optlen = sizeof(optval);
  auto const retval =
      ::getsockopt(*native_handle(), SOL_SOCKET, SO_RCVBUF, &optval, &optlen);

  if (retval == platform::kErrorRetval || optlen != sizeof(optval))
      [[unlikely]] {
    throw std::runtime_error(
        std::format("get rcvbuf failed: {}", platform::errno_strerror()));
  }

  *option = static_cast<std::uint32_t>(std::max(0, optval));
}

void base_socket::get_option(reuseaddr & option) const {
  int optval{};
  platform::socklen_t optlen = sizeof(optval);
  auto const retval = ::getsockopt(*native_handle(), SOL_SOCKET, SO_REUSEADDR,
                                   &optval, &optlen);

  if (retval == platform::kErrorRetval || optlen != sizeof(optval))
      [[unlikely]] {
    throw std::runtime_error(
        std::format("get reuseaddr failed: {}", platform::errno_strerror()));
  }

  *option = (optval != 0);
}

void base_socket::get_option(reuseport & option) const {
  int optval{};
  platform::socklen_t optlen = sizeof(optval);
  auto const retval = ::getsockopt(*native_handle(), SOL_SOCKET, SO_REUSEPORT,
                                   &optval, &optlen);
  if (retval == platform::kErrorRetval || optlen != sizeof(optval))
      [[unlikely]] {
    throw std::runtime_error(
        std::format("get reuseport failed: {}", platform::errno_strerror()));
  }

  *option = (optval != 0);
}

void base_socket::get_option(sndbuf & option) const {
  int optval{};
  platform::socklen_t optlen = sizeof(optval);
  auto const retval =
      ::getsockopt(*native_handle(), SOL_SOCKET, SO_SNDBUF, &optval, &optlen);

  if (retval == platform::kErrorRetval || optlen != sizeof(optval))
      [[unlikely]] {
    throw std::runtime_error(
        std::format("get sndbuf failed: {}", platform::errno_strerror()));
  }

  *option = static_cast<std::uint32_t>(std::max(0, optval));
}

auto base_socket::bind(base_sockaddr const & sockaddr) noexcept(false)
    -> bind_status {
  auto const retval =
      ::bind(*native_handle(),
             static_cast<::sockaddr const *>(sockaddr.native_sockaddr()),
             static_cast<platform::socklen_t>(sockaddr.length()));

  if (retval == platform::kErrorRetval) [[unlikely]] {
    if (platform::get_last_error() == std::errc::address_in_use) {
      return bind_status::kInUse;
    }

    throw std::runtime_error(
        std::format("failed to bind socket: {}", platform::errno_strerror()));
  }

  return bind_status::kSuccess;
}

auto base_socket::connect(base_sockaddr const & sockaddr) -> connect_status {
  auto const retval =
      ::connect(*native_handle(),
                static_cast<::sockaddr const *>(sockaddr.native_sockaddr()),
                static_cast<platform::socklen_t>(sockaddr.length()));

  if (retval == platform::kErrorRetval) {
    switch (platform::get_last_error()) {
      case std::errc::already_connected:
        break;
      case std::errc::operation_in_progress:
      case std::errc::resource_unavailable_try_again:
      case std::errc::connection_already_in_progress:
        return connect_status::kPending;
      case std::errc::connection_refused:
      case std::errc::no_such_file_or_directory:
        return connect_status::kRefused;
      [[unlikely]] default:
        throw std::runtime_error(std::format(
            "failed to connect base_socket: {}", platform::errno_strerror()));
    }
  }

  return connect_status::kSuccess;
}

void base_socket::getsockname(base_sockaddr & sockaddr) const {
  auto socklen      = static_cast<platform::socklen_t>(sockaddr.length());
  auto const retval = ::getsockname(
      *native_handle(), static_cast<::sockaddr *>(sockaddr.native_sockaddr()),
      &socklen);

  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("getsockname failed: {}", platform::errno_strerror()));
  }
}

void base_socket::getpeername(base_sockaddr & sockaddr) const {
  auto socklen      = static_cast<platform::socklen_t>(sockaddr.length());
  auto const retval = ::getpeername(
      *native_handle(), static_cast<::sockaddr *>(sockaddr.native_sockaddr()),
      &socklen);

  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("getpeername failed: {}", platform::errno_strerror()));
  }
}

void base_socket::listen(std::size_t backlog) {
  auto const retval = ::listen(*native_handle(), static_cast<int>(backlog));

  if (retval == platform::kErrorRetval) [[unlikely]] {
    throw std::runtime_error(
        std::format("listen failed: {}", platform::errno_strerror()));
  }
}

auto base_socket::accept(base_socket & socket) const -> accept_status {
  platform::native_handle_t const handle = platform::accept(*native_handle());

  if (handle == platform::kInvalidNativeHandle) [[unlikely]] {
    std::errc const error = platform::get_last_error();
    if (error == std::errc::resource_unavailable_try_again ||
        error == std::errc::operation_would_block) {
      return accept_status::kEmptyQueue;
    }

    throw std::runtime_error(
        std::format("accept failed: {}", platform::errno_strerror()));
  }

  *socket.native_handle() = handle;

  return accept_status::kSuccess;
}

auto base_socket::send(std::span<std::byte const> bytes) noexcept(false)
    -> std::size_t {
  auto const retval = ::send(*native_handle(), bytes.data(), bytes.size(), 0);

  if (retval == platform::kErrorRetval) [[unlikely]] {
    std::errc const error = platform::get_last_error();
    if (error == std::errc::resource_unavailable_try_again ||
        error == std::errc::operation_would_block ||
        error == std::errc::no_buffer_space) {
      return 0;
    }

    throw std::runtime_error(
        std::format("send failed: {}", platform::errno_strerror()));
  }

  return static_cast<std::size_t>(retval);
}

auto base_socket::sendto(std::span<std::byte const> bytes,
                         base_sockaddr const & sockaddr) -> std::size_t {
  auto const retval =
      ::sendto(*native_handle(), bytes.data(), bytes.size(), 0,
               static_cast<::sockaddr const *>(sockaddr.native_sockaddr()),
               static_cast<platform::socklen_t>(sockaddr.length()));

  if (retval == platform::kErrorRetval) [[unlikely]] {
    std::errc const error = platform::get_last_error();
    if (error == std::errc::resource_unavailable_try_again ||
        error == std::errc::operation_would_block ||
        error == std::errc::no_buffer_space) {
      return 0;
    }

    throw std::runtime_error(
        std::format("sendto failed: {}", platform::errno_strerror()));
  }

  return static_cast<std::size_t>(retval);
}

auto base_socket::recv(std::span<std::byte> bytes) -> std::size_t {
  auto const retval = ::recv(*native_handle(), bytes.data(), bytes.size(), 0);

  if (retval == platform::kErrorRetval) [[unlikely]] {
    std::errc const error = platform::get_last_error();
    if (error == std::errc::resource_unavailable_try_again ||
        error == std::errc::operation_would_block) {
      return 0;
    }

    throw std::runtime_error(
        std::format("recv failed: {}", platform::errno_strerror()));
  }

  return static_cast<std::size_t>(retval);
}

auto base_socket::recvfrom(std::span<std::byte> bytes, base_sockaddr & sockaddr)
    -> std::size_t {
  auto socklen      = static_cast<platform::socklen_t>(sockaddr.length());
  auto const retval = ::recvfrom(
      *native_handle(), bytes.data(), bytes.size(), 0,
      static_cast<::sockaddr *>(sockaddr.native_sockaddr()), &socklen);

  if (retval == platform::kErrorRetval) [[unlikely]] {
    std::errc const error = platform::get_last_error();
    if (error == std::errc::resource_unavailable_try_again ||
        error == std::errc::operation_would_block) {
      return 0;
    }

    throw std::runtime_error(
        std::format("recvfrom failed: {}", platform::errno_strerror()));
  }

  return static_cast<std::size_t>(retval);
}

}  // namespace skynet
