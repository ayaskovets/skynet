#pragma once

namespace skynet {

class uninitialized_t final {};

/**
 * @brief A type tag used for explicitly constructing uninitialized values e.g.
 * a socket or a sockaddr that is going to be used as an out parameter
 */
constexpr uninitialized_t uninitialized{};

}  // namespace skynet
