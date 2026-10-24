#pragma once

#include <chrono>

#include <swiss/functional.hpp>
#include <swiss/mixins.hpp>
#include <swiss/static_pimpl.hpp>

#include "base_socket.hpp"

namespace skynet {

/**
 * @brief IO multiplexing class
 * @warning Does not own sockets being polled for IO events. Each socket added
 * for polling must be erased before it gets closed to prevent UB
 */
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class poller final : public swiss::non_copyable {
 public:
  enum event_filter : std::uint8_t {
    kPollIn  = 1U << 0U,  ///< There is data to read
    kPollOut = 1U << 2U,  ///< There is available space to write
    kPollErr = 1U << 3U,  ///< Error condition, used only in output
    kPollHup = 1U << 4U,  ///< Connection hang up, used only in output
  };

 public:
  explicit poller(std::size_t capacity);
  poller(poller &&) noexcept;
  auto operator=(poller &&) noexcept -> poller &;
  ~poller() noexcept;

 public:
  /**
   * @brief Track or update the selected events for the socket for future
   * polling
   * @warning Not safe to call from the callback
   * @warning Not thread-safe
   */
  void insert_or_assign(base_socket const & sock, event_filter filter,
                        void * data);

  /**
   * @brief Remove socket from polling list
   */
  void erase(base_socket const & sock);

 public:
  /**
   * @brief Synchronously poll for the events on tracked file descriptors
   */
  void poll_for(std::chrono::milliseconds milliseconds);
  void try_poll();
  void poll();

 private:
  struct impl;
  swiss::static_pimpl<impl, 32, 8> pimpl_;
};

}  // namespace skynet
