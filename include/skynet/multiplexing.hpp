#pragma once

#include <chrono>
#include <span>

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
  enum event : std::uint8_t {
    kPollIn   = 1U << 0U,  ///< There is data to read
    kPollPri  = 1U << 1U,  ///< There is OOB data to read
    kPollOut  = 1U << 2U,  ///< There is available space to write
    kPollErr  = 1U << 3U,  ///< Error condition
    kPollHup  = 1U << 4U,  ///< Connection hang up
    kPollNval = 1U << 5U,  ///< Invalid fd
  };
  using events_type = std::underlying_type_t<event>;

  // NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
  class ticket final : public swiss::non_copyable {
   private:
    explicit constexpr ticket(std::size_t index) noexcept : index_(index) {}

   public:
    constexpr ticket(ticket && that) noexcept
        : index_(std::exchange(that.index_, std::dynamic_extent)) {}

    constexpr auto operator=(ticket && that) noexcept -> ticket & {
      std::swap(index_, that.index_);
      return *this;
    }

   private:
    std::size_t index_;

   private:
    [[nodiscard]] constexpr auto is_moved_from() const noexcept -> bool {
      return index_ == std::dynamic_extent;
    }

   private:
    /**
     * @note For poller to create and access the underlying ticket value
     */
    friend class poller;
  };

 public:
  poller() noexcept;
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

  /**
   * @brief Non-binding request to shrink the underlying buffers
   */
  void shrink_to_fit() noexcept;

 public:
  /**
   * @brief Track the selected events for the socket for future polling
   * @warning Not safe to call from the callback
   * @warning Not thread-safe
   * @return Ticket that should be used to erase the socket from poller before
   * socket is destroyed
   */
  auto insert(base_socket const & sock, events_type events) -> ticket;

  /**
   * @brief Track the selected events for the already inserted socket for future
   * polling
   * @warning Not safe to call from the callback
   * @warning Not thread-safe
   */
  void update(ticket const & ticket, events_type events);

  /**
   * @brief Remove socket from polling list
   */
  void erase(ticket ticket);

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
};

}  // namespace skynet
