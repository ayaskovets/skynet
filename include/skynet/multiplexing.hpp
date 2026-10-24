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

  class event_iterator final {
   public:
    struct value_type final {
      event_filter filter;
      void * data;
    };
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

   public:
    constexpr event_iterator() noexcept : poller_(nullptr), index_(0) {}
    constexpr event_iterator(poller const * owner, std::size_t index) noexcept
        : poller_(owner), index_(index) {}

   public:
    constexpr auto operator++() noexcept -> event_iterator & {
      return ++index_, *this;
    }

    constexpr auto operator++(int) noexcept -> event_iterator {
      return {poller_, index_++};
    }

   public:
    [[nodiscard]] auto operator*() const noexcept -> value_type;

    constexpr auto operator==(event_iterator const &) const noexcept
        -> bool = default;

   private:
    poller const * poller_;
    std::size_t index_;
  };
  using event_range_type =
      std::ranges::subrange<event_iterator, event_iterator,
                            std::ranges::subrange_kind::sized>;

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
   * @brief Remove the filter from polling for the socket
   */
  void erase(base_socket const & sock, event_filter filter);

 public:
  /**
   * @brief Synchronously poll for the events on tracked file descriptors
   * @warning Returned iterators are valid until the next poll
   */
  auto poll(std::optional<std::chrono::milliseconds> milliseconds)
      -> event_range_type;

 private:
  struct impl;
  swiss::static_pimpl<impl, 32, 8> pimpl_;
};

}  // namespace skynet
