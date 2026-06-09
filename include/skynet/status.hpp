#pragma once

#include <cstdint>

namespace skynet {

/**
 * @brief Return code for bind()
 */
enum class bind_status : std::uint8_t {
  kSuccess,  ///< Successfully bound socket
  kInUse,    ///< Sockaddr already in use
};

/**
 * @brief Return code for connect()
 */
enum class connect_status : std::uint8_t {
  kSuccess,  ///< Successfully connected socket
  kPending,  ///< Connection pending for a nonblocking socket
  kRefused,  ///< Connection refused
};

/**
 * @brief Return code for accept()
 */
enum class accept_status : std::uint8_t {
  kSuccess,     ///< Successfully accepted a new connection
  kEmptyQueue,  ///< No incoming connections
};

}  // namespace skynet
