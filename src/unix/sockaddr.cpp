#include <skynet/unix/sockaddr.hpp>

#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet::unix {

// NOLINTNEXTLINE(bugprone-exception-escape)
auto sockaddr::kInvalid() noexcept -> sockaddr const & {
  static sockaddr const sockaddr("/");
  return sockaddr;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
auto sockaddr::kEmpty() noexcept -> sockaddr const & {
  static sockaddr const sockaddr("");
  return sockaddr;
}

sockaddr::sockaddr(std::filesystem::path const & path)
    : base_sockaddr(family::kUnix) {
  constexpr std::size_t kMaxPathSize = sizeof(sockaddr_un{}.sun_path) - 1;

  if (path.native().size() >= kMaxPathSize) [[unlikely]] {
    throw std::invalid_argument(
        std::format("invalid path size {}", path.native().size()));
  }

  auto * storage = static_cast<sockaddr_un *>(native_sockaddr());
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
  std::strncpy(storage->sun_path, path.c_str(),
               std::min(sizeof(storage->sun_path), path.native().size()));
}

auto sockaddr::get_path() const noexcept -> std::filesystem::path {
  auto const * storage = static_cast<sockaddr_un const *>(native_sockaddr());
  return {storage->sun_path};
}

}  // namespace skynet::unix
