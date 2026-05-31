#pragma once

#include <string_view>
#include <variant>
#include <vector>

#include "inet/sockaddr.hpp"
#include "inet6/sockaddr.hpp"
#include "socktype.hpp"

namespace skynet {

/**
 * @brief Resolve IPv4 or IPv6 address from a hostname
 * @note Use family::kUnspec to resolve all available addresses
 * @note Use socktype::kUnspec to resolve all available socket types
 */
auto resolve(std::string_view hostname, family family, socktype socktype)
    -> std::vector<std::variant<inet::sockaddr, inet6::sockaddr>>;

}  // namespace skynet
