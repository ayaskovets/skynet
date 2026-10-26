#pragma once

#include <chrono>
#include <filesystem>
#include <optional>
#include <span>

#include <swiss/concepts.hpp>
#include <swiss/os_kind.hpp>

/**
 * @brief Platform-specific types, functions and variables are implicitly used
 * throughout the library code and have to be defined in the corresponding
 * platform header. The implementation must reside in the 'skynet::platform'
 * namespace
 */
#if (SWISS_OS_KIND == kMacOS)
#include "bsd.hpp"
#endif

namespace skynet::platform::concepts {

template <typename Platform = void>
concept is_valid_platform_implementaiton = requires(
    platform::native_handle_type socket, platform::socklen_t socklen,
    int family, int type, int protocol, int optval, std::filesystem::path path,
    platform::native_handle_type reactor, platform::event_filter_type filter,
    platform::event_type event, std::span<platform::event_type> events,
    void * data, std::optional<std::chrono::milliseconds> timeout) {
  /**
   * @section Types
   */
  { socket } -> swiss::not_same_as<void>;
  { socklen } -> swiss::not_same_as<void>;
  { reactor } -> swiss::not_same_as<void>;
  { filter } -> swiss::not_same_as<void>;
  { event } -> swiss::not_same_as<void>;

  /**
   * @section Constants
   */
  {
    platform::kInvalidNativeHandle
  } -> swiss::decay_same_as<platform::native_handle_type>;
  { platform::kErrorRetval } -> swiss::decay_same_as<int>;
  { platform::kPollIn } -> swiss::decay_same_as<platform::event_filter_type>;
  { platform::kPollOut } -> swiss::decay_same_as<platform::event_filter_type>;
  { platform::kPollErr } -> swiss::decay_same_as<platform::event_filter_type>;
  { platform::kPollHup } -> swiss::decay_same_as<platform::event_filter_type>;

  /**
   * @section Functions
   */
  {
    platform::create_socket(family, type, protocol)
  } -> std::same_as<platform::native_handle_type>;
  { platform::copy_handle(socket) } -> std::same_as<int>;
  { platform::close_handle(socket) } -> std::same_as<int>;
  { platform::set_nonblock(socket, optval) } -> std::same_as<int>;
  { platform::get_nonblock(socket) } -> std::same_as<int>;
  { platform::accept(socket) } -> std::same_as<platform::native_handle_type>;
  { platform::unlink(path) } -> std::same_as<int>;
  { platform::get_last_error() } -> std::same_as<std::errc>;
  { platform::errno_strerror() } -> std::same_as<std::string>;
  { platform::create_reactor() } -> std::same_as<platform::native_handle_type>;
  { platform::set_filter(reactor, socket, filter, data) } -> std::same_as<int>;
  { platform::remove_filter(reactor, socket) } -> std::same_as<int>;
  { platform::poll_reactor(reactor, events, timeout) } -> std::same_as<int>;
  { platform::get_event_filter(event) } -> std::same_as<event_filter_type>;
  { platform::get_event_data(event) } -> std::same_as<void *>;

  /**
   * @section Exceptions
   */
  requires noexcept(platform::create_socket(family, type, protocol));
  requires noexcept(platform::copy_handle(socket));
  requires noexcept(platform::close_handle(socket));
  requires noexcept(platform::set_nonblock(socket, optval));
  requires noexcept(platform::get_nonblock(socket));
  requires noexcept(platform::accept(socket));
  requires noexcept(platform::unlink(path));
  requires noexcept(platform::get_last_error());
  requires noexcept(platform::errno_strerror());
  requires noexcept(platform::create_reactor());
  requires noexcept(platform::set_filter(reactor, socket, filter, data));
  requires noexcept(platform::remove_filter(reactor, socket));
  requires noexcept(platform::poll_reactor(reactor, events, timeout));
  requires noexcept(platform::get_event_filter(event));
  requires noexcept(platform::get_event_data(event));
};
static_assert(is_valid_platform_implementaiton<>,
              "invalid platform implementation");

}  // namespace skynet::platform::concepts
