#pragma once

#include <string_view>
#include <variant>
#include <vector>

#include "inet/sockaddr.hpp"
#include "inet6/sockaddr.hpp"
#include "protocol.hpp"

namespace skynet {

/**
 * @brief Resolve IPv4 or IPv6 address from a hostname
 * @note Use family::kUnspecified to resolve all available addresses
 * @note Use protocol::kUnspecified to resolve all available protocols
 */
auto resolve(std::string_view hostname, family family, protocol protocol)
    -> std::vector<std::variant<inet::sockaddr, inet6::sockaddr>>;

}  // namespace skynet
