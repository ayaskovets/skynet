#include <skynet/base_sockaddr.hpp>

#include <utility>

#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet {

base_sockaddr::base_sockaddr(enum family family)
    : impl_([family] -> swiss::unchecked_any {
        /**
         * @warning Additional sockaddr families must be implemented throughout
         * the whole class otherwise its operations invoke UB
         */
        switch (family) {
          case family::kUnspec:
            throw std::invalid_argument(
                "base_sockaddr::base_sockaddr(): invalid sockaddr family");
          case family::kInet:
            return swiss::unchecked_any(::sockaddr_in{.sin_family = AF_INET});
          case family::kInet6:
            return swiss::unchecked_any(
                ::sockaddr_in6{.sin6_family = AF_INET6});
          case family::kUnix:
            return swiss::unchecked_any(::sockaddr_un{.sun_family = AF_UNIX});
        }
      }()) {}

auto base_sockaddr::operator==(base_sockaddr const & that) const noexcept
    -> bool {
  switch (impl_.get<::sockaddr>()->sa_family) {
    case AF_INET: {
      auto const * this_sockaddr = impl_.get<::sockaddr_in>();
      auto const * that_sockaddr = that.impl_.get<::sockaddr_in>();

      return std::memcmp(&this_sockaddr->sin_addr, &that_sockaddr->sin_addr,
                         sizeof(::sockaddr_in{}.sin_addr)) == 0 &&
             this_sockaddr->sin_port == that_sockaddr->sin_port;
    }
    case AF_INET6: {
      auto const * this_sockaddr = impl_.get<::sockaddr_in6>();
      auto const * that_sockaddr = that.impl_.get<::sockaddr_in6>();

      return std::memcmp(&this_sockaddr->sin6_addr, &that_sockaddr->sin6_addr,
                         sizeof(::sockaddr_in6{}.sin6_addr)) == 0 &&
             this_sockaddr->sin6_port == that_sockaddr->sin6_port;
    }
    case AF_UNIX: {
      auto const * this_sockaddr = impl_.get<::sockaddr_un>();
      auto const * that_sockaddr = that.impl_.get<::sockaddr_un>();

      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
      return std::strncmp(this_sockaddr->sun_path, that_sockaddr->sun_path,
                          sizeof(::sockaddr_un{}.sun_path)) == 0;
    }
    default:
      /**
       * @note Family switch is handled in the constructor
       */
      std::unreachable();
  }
}

auto base_sockaddr::socklen() const noexcept -> std::size_t {
  switch (impl_.get<::sockaddr>()->sa_family) {
    case AF_INET:
      return sizeof(::sockaddr_in);
    case AF_INET6:
      return sizeof(::sockaddr_in6);
    case AF_UNIX:
      return sizeof(::sockaddr_un);
    default:
      /**
       * @note Family switch is handled in the constructor
       */
      std::unreachable();
  }
}

auto base_sockaddr::family() const noexcept -> enum family {
  switch (impl_.get<::sockaddr>()->sa_family) {
    case AF_INET:
      return family::kInet;
    case AF_INET6:
      return family::kInet6;
    case AF_UNIX:
      return family::kUnix;
    default:
      /**
       * @note Family switch is handled in the constructor
       */
      std::unreachable();
  }
}

auto base_sockaddr::to_string() const -> std::string {
  constexpr std::size_t kPortStrlen    = 6;
  constexpr std::size_t kColonStrlen   = 1;
  constexpr std::size_t kBracketStrlen = 1;

  std::string string;
  switch (impl_.get<::sockaddr>()->sa_family) {
    case AF_INET: {
      auto const * sockaddr = impl_.get<::sockaddr_in>();

      string.resize(INET_ADDRSTRLEN + kColonStrlen + kPortStrlen);

      if (inet_ntop(AF_INET, &sockaddr->sin_addr, string.data(),
                    static_cast<platform::socklen_t>(string.size())) == nullptr)
          [[unlikely]] {
        throw std::runtime_error(
            std::format("base_sockaddr::to_string(): inet_ntop error {}",
                        platform::errno_strerror()));
      }

      string.resize(std::min(string.capacity(), string.find('\0')));
      string.push_back(':');
      string.append(std::to_string(ntohs(sockaddr->sin_port)));
      break;
    }
    case AF_INET6: {
      auto const * sockaddr = impl_.get<::sockaddr_in6>();

      string.resize(kBracketStrlen + INET6_ADDRSTRLEN + kBracketStrlen +
                    kColonStrlen + kPortStrlen);
      string.front() = '[';

      if (inet_ntop(
              AF_INET6, &sockaddr->sin6_addr,
              // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
              string.data() + kBracketStrlen,
              static_cast<platform::socklen_t>(string.size())) == nullptr)
          [[unlikely]] {
        throw std::runtime_error(
            std::format("base_sockaddr::to_string(): inet_ntop error {}",
                        platform::errno_strerror()));
      }

      string.resize(std::min(string.capacity(), string.find('\0')));
      string.push_back(']');
      string.push_back(':');
      string.append(std::to_string(ntohs(sockaddr->sin6_port)));
      break;
    }
    case AF_UNIX: {
      auto const * sockaddr = impl_.get<::sockaddr_un>();

      string.resize(sizeof(sockaddr->sun_path));
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
      string.assign(sockaddr->sun_path, sizeof(sockaddr->sun_path));
      string.resize(std::min(string.capacity(), string.find('\0')));
      break;
    }
    default:
      /**
       * @note Family switch is handled in the constructor
       */
      std::unreachable();
  }
  return string;
}

auto base_sockaddr::native_sockaddr() noexcept -> void * {
  return impl_.get<void>();
}

auto base_sockaddr::native_sockaddr() const noexcept -> void const * {
  return impl_.get<void>();
}

}  // namespace skynet