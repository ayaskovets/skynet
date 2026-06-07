#include <skynet/unix/base_socket.hpp>

#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet::unix {

base_socket::base_socket(uninitialized_t) noexcept
    : skynet::base_socket(uninitialized_t{}) {}

base_socket::base_socket(skynet::socktype type)
    : skynet::base_socket(skynet::family::kUnix, type,
                          skynet::protocol::kDefault) {}

base_socket::~base_socket() noexcept {
  if (*this == base_socket(uninitialized_t{})) {
    return;
  }

  skynet::unix::sockaddr sockaddr(skynet::unix::sockaddr::kEmpty());
  try {
    getsockname(sockaddr);
  } catch (...) {
    return;
  }

  if (!sockaddr.get_path().empty()) {
    std::ignore = platform::unlink(sockaddr.get_path());
  }
}

auto base_socket::unlink_bind(skynet::unix::sockaddr const & sockaddr)
    -> skynet::bind_status {
  auto const retval = platform::unlink(sockaddr.get_path());
  if (retval == platform::kErrorRetval) {
    if (platform::get_last_error() != std::errc::no_such_file_or_directory) {
      throw std::runtime_error(
          std::format("unlink failed: {}", platform::errno_strerror()));
    }
  }
  return skynet::base_socket::bind(sockaddr);
}

void base_socket::unlink_close() {
  if (*this == base_socket(uninitialized_t{})) {
    return;
  }

  skynet::unix::sockaddr sockaddr(skynet::unix::sockaddr::kEmpty());
  getsockname(sockaddr);
  if (!sockaddr.get_path().empty()) {
    auto const retval = platform::unlink(sockaddr.get_path());
    if (retval == platform::kErrorRetval) {
      if (platform::get_last_error() != std::errc::no_such_file_or_directory) {
        throw std::runtime_error(
            std::format("unlink failed: {}", platform::errno_strerror()));
      }
    }
  }

  skynet::base_socket::close();
}

}  // namespace skynet::unix
