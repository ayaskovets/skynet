#pragma once

#include <chrono>

#include <swiss/functional.hpp>
#include <swiss/mixins.hpp>
#include <swiss/static_pimpl.hpp>

#include "base_socket.hpp"

namespace skynet {

/**
 * @brief IO multiplexing class
 */
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class poller final : public swiss::non_copyable {
 public:
  enum event : std::uint8_t {
    kPollIn   = 1U << 0U,  ///< There is data to read
    kPollPri  = 1U << 1U,  ///< There is OOB data to read
    kPollOut  = 1U << 2U,  ///< There is available space to write
    kPollErr  = 1U << 3U,  ///< Error condition
    kPollHup  = 1U << 4U,  ///< Connection hang up
    kPollNval = 1U << 5U,  ///< Invalid fd
  };
  using events_t = std::underlying_type_t<event>;

 public:
  enum class callback_result : std::uint8_t {
    kClose,      ///< Close the socket after the callback exit
    kDoNothing,  ///< Do nothing after the callback exit
  };
  using callback_t =
      swiss::noexcept_function<callback_result, base_socket const &, events_t>;

 public:
  explicit poller(callback_t const & callback);
  poller(poller &&) noexcept;
  auto operator=(poller &&) noexcept -> poller &;
  ~poller() noexcept;

 public:
  /**
   * @brief Return the capacity of the underlying storage in number of sockets
   */
  [[nodiscard]] auto capacity() const noexcept -> std::size_t;

  /**
   * @brief Return the number of sockets that are polled
   */
  [[nodiscard]] auto size() const noexcept -> std::size_t;

 public:
  /**
   * @brief Poll for selected events for the socket
   * @note Safe to call from the callback
   * @return True if the socket was inserted
   */
  void insert(base_socket sock, events_t events);

 public:
  /**
   * @brief Non-binding request to shrink the underlying buffers
   */
  void shrink_to_fit();

 public:
  /**
   * @brief Synchronously poll for the events on tracked file descriptors
   */
  void poll_for(std::chrono::milliseconds milliseconds);
  void try_poll();
  void poll();

 private:
  struct impl;
  swiss::static_pimpl<impl, 48, 8> pimpl_;
  callback_t callback_;
};

}  // namespace skynet
