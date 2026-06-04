#pragma once

#include <concepts>
#include <span>

#include "family.hpp"
#include "options.hpp"
#include "sockaddr.hpp"
#include "socktype.hpp"
#include "status.hpp"
#include "uninitialized.hpp"

namespace skynet::concepts {

template <typename T, typename Sockaddr>
concept socket = requires(T socket, Sockaddr sockaddr) {
  { socket.family() } -> std::same_as<family>;
  { socket.socktype() } -> std::same_as<socktype>;

  { socket.bind(sockaddr) } -> std::same_as<bind_status>;
  { socket.connect(sockaddr) } -> std::same_as<connect_status>;

  { socket.getsockname(sockaddr) };
  { socket.getpeername(sockaddr) };

  { socket.close() };

  requires requires(broadcast option) {
    { socket.set_option(option) } -> std::same_as<void>;
    { socket.get_option(option) } -> std::same_as<void>;
  };
  requires requires(keepalive option) {
    { socket.set_option(option) } -> std::same_as<void>;
    { socket.get_option(option) } -> std::same_as<void>;
  };
  requires requires(linger option) {
    { socket.set_option(option) } -> std::same_as<void>;
    { socket.get_option(option) } -> std::same_as<void>;
  };
  requires requires(nodelay option) {
    { socket.set_option(option) } -> std::same_as<void>;
    { socket.get_option(option) } -> std::same_as<void>;
  };
  requires requires(nonblock option) {
    { socket.set_option(option) } -> std::same_as<void>;
    { socket.get_option(option) } -> std::same_as<void>;
  };
  requires requires(oobinline option) {
    { socket.set_option(option) } -> std::same_as<void>;
    { socket.get_option(option) } -> std::same_as<void>;
  };
  requires requires(rcvbuf option) {
    { socket.set_option(option) } -> std::same_as<void>;
    { socket.get_option(option) } -> std::same_as<void>;
  };
  requires requires(reuseaddr option) {
    { socket.set_option(option) } -> std::same_as<void>;
    { socket.get_option(option) } -> std::same_as<void>;
  };
  requires requires(reuseport option) {
    { socket.set_option(option) } -> std::same_as<void>;
    { socket.get_option(option) } -> std::same_as<void>;
  };
  requires requires(sndbuf option) {
    { socket.set_option(option) } -> std::same_as<void>;
    { socket.get_option(option) } -> std::same_as<void>;
  };

  requires skynet::concepts::sockaddr<Sockaddr>;
  requires std::constructible_from<T, uninitialized_t>;
  requires std::is_default_constructible_v<T>;
  requires std::equality_comparable<T>;
  requires std::copy_constructible<T>;
  requires std::move_constructible<T>;
  requires std::is_copy_assignable_v<T>;
  requires std::is_move_assignable_v<T>;
};

template <typename T, typename Sockaddr>
concept stream_socket =
    socket<T, Sockaddr> &&
    requires(T socket, Sockaddr sockaddr, std::span<std::byte> bytes) {
      { socket.listen(std::size_t{}) };
      { socket.accept(socket) } -> std::same_as<accept_status>;

      { socket.send(bytes) } -> std::same_as<size_t>;
      { socket.recv(bytes) } -> std::same_as<size_t>;
    };

template <typename T, typename Sockaddr>
concept dgram_socket =
    socket<T, Sockaddr> &&
    requires(T socket, Sockaddr sockaddr, std::span<std::byte> bytes) {
      { socket.sendto(bytes, sockaddr) } -> std::same_as<size_t>;
      { socket.recvfrom(bytes, sockaddr) } -> std::same_as<size_t>;
    };

}  // namespace skynet::concepts
