#pragma once

#include <cstdint>

namespace skynet {

/**
 * @brief Return code for bind()
 */
enum class bind_status : std::uint8_t {
  kSuccess,
  kInUse,
};

/**
 * @brief Return code for connect()
 */
enum class connect_status : std::uint8_t {
  kSuccess,
  kPending,
  kRefused,
};

/**
 * @brief Return code for accept()
 */
enum class accept_status : std::uint8_t {
  kSuccess,
  kEmptyQueue,
};

}  // namespace skynet
