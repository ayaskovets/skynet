#pragma once

#include <chrono>
#include <iterator>
#include <ranges>

#include <swiss/functional.hpp>
#include <swiss/mixins.hpp>
#include <swiss/static_pimpl.hpp>

#include "base_socket.hpp"

namespace skynet {

/**
 * @brief Edge-triggered IO reactor
 * @warning Does not own sockets being polled for IO events. Each socket added
 * for polling must be erased before it gets closed to prevent UB
 */
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
class reactor final : public swiss::non_copyable {
 public:
  enum event_filter : std::uint8_t {
    kPollIn  = 1U << 0U,  ///< There is data to read
    kPollOut = 1U << 2U,  ///< There is available space to write
    kPollErr = 1U << 3U,  ///< Error condition, used only in output
    kPollHup = 1U << 4U,  ///< Connection hang up, used only in output
  };

  class event_iterator final {
   public:
    struct value_type final {
      event_filter filter;
      void * data;
    };
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

   public:
    constexpr event_iterator() noexcept : reactor_(nullptr), index_(0) {}
    constexpr event_iterator(reactor const * owner, std::size_t index) noexcept
        : reactor_(owner), index_(index) {}

   public:
    constexpr auto operator++() noexcept -> event_iterator & {
      return ++index_, *this;
    }

    constexpr auto operator++(int) noexcept -> event_iterator {
      return {reactor_, index_++};
    }

   public:
    [[nodiscard]] auto operator*() const noexcept -> value_type;

    constexpr auto operator==(event_iterator const &) const noexcept
        -> bool = default;

   private:
    reactor const * reactor_;
    std::size_t index_;
  };
  using event_range_type =
      std::ranges::subrange<event_iterator, event_iterator,
                            std::ranges::subrange_kind::sized>;

 public:
  explicit reactor(std::size_t capacity);
  reactor(reactor &&) noexcept;
  auto operator=(reactor &&) noexcept -> reactor &;
  ~reactor() noexcept;

 public:
  /**
   * @brief Replace the complete interest mask and shared data for the socket
   * @note Unselected directions are removed; an empty mask removes the socket
   * @note Notifications are edge-triggered and registrations are persistent
   */
  void insert_or_assign(base_socket const & sock, event_filter filter,
                        void * data);

  /**
   * @brief Remove the socket from polling
   */
  void erase(base_socket const & sock);

 public:
  /**
   * @brief Synchronously poll for the events on tracked file descriptors
   * @note A socket may produce separate read and write events
   * @warning Returned iterators are valid until the next poll
   */
  auto poll(std::optional<std::chrono::milliseconds> milliseconds)
      -> event_range_type;

 private:
  struct impl;
  swiss::static_pimpl<impl, 32, 8> pimpl_;
};

}  // namespace skynet
