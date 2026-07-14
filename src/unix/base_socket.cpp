#include <skynet/unix/base_socket.hpp>

#include "../base_socket_impl.hpp"  // IWYU pragma: keep
#include "platform/platform.hpp"    // IWYU pragma: keep

namespace skynet::unix {

base_socket::base_socket(uninitialized_t) noexcept
    : skynet::base_socket(uninitialized) {}

base_socket::base_socket(skynet::socktype type)
    : skynet::base_socket(skynet::family::kUnix, type,
                          skynet::protocol::kDefault) {}

base_socket::~base_socket() noexcept {
  if (*native_handle() == platform::kInvalidNativeHandle) {
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

auto base_socket::bind(skynet::unix::sockaddr const & sockaddr)
    -> skynet::bind_status {
  switch (skynet::base_socket::bind(sockaddr)) {
    case skynet::bind_status::kInUse: {
      reuseaddr option{};
      get_option(option);
      if (*option) {
        return unlink_bind(sockaddr);
      }
      return skynet::bind_status::kInUse;
    }
    case skynet::bind_status::kSuccess:
      return skynet::bind_status::kSuccess;
  }
}

auto base_socket::unlink_bind(skynet::unix::sockaddr const & sockaddr)
    -> skynet::bind_status {
  auto const retval = platform::unlink(sockaddr.get_path());
  if (retval == platform::kErrorRetval) {
    if (platform::get_last_error() != std::errc::no_such_file_or_directory) {
      throw std::runtime_error(
          std::format("base_socket::unlink_bind(): unlink error {}",
                      platform::errno_strerror()));
    }
  }
  return skynet::base_socket::bind(sockaddr);
}

void base_socket::unlink_close() {
  if (*native_handle() == platform::kInvalidNativeHandle) {
    return;
  }

  skynet::unix::sockaddr sockaddr(skynet::unix::sockaddr::kEmpty());
  getsockname(sockaddr);
  if (!sockaddr.get_path().empty()) {
    auto const retval = platform::unlink(sockaddr.get_path());
    if (retval == platform::kErrorRetval) {
      if (platform::get_last_error() != std::errc::no_such_file_or_directory) {
        throw std::runtime_error(
            std::format("base_socket::unlink_bind(): unlink error {}",
                        platform::errno_strerror()));
      }
    }
  }

  skynet::base_socket::close();
}

}  // namespace skynet::unix
