#pragma once

#include <concepts>
#include <span>

#include "family.hpp"
#include "protocol.hpp"
#include "sockaddr.hpp"
#include "socktype.hpp"
#include "status.hpp"
#include "uninitialized.hpp"

namespace skynet::concepts {

template <typename T, typename Sockaddr>
concept socket = requires(T socket, Sockaddr sockaddr) {
  { socket.family() } -> std::same_as<family>;
  { socket.type() } -> std::same_as<socktype>;
  { socket.protocol() } -> std::same_as<protocol>;

  { socket.bind(sockaddr) } -> std::same_as<bind_status>;
  { socket.connect(sockaddr) } -> std::same_as<connect_status>;

  { socket.bind_sockaddr(sockaddr) };
  { socket.connect_sockaddr(sockaddr) };

  { socket.close() };

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
      { socket.receive(bytes) } -> std::same_as<size_t>;
    };

template <typename T, typename Sockaddr>
concept dgram_socket =
    socket<T, Sockaddr> &&
    requires(T socket, Sockaddr sockaddr, std::span<std::byte> bytes) {
      { socket.send_to(bytes, sockaddr) } -> std::same_as<size_t>;
      { socket.receive_from(bytes, sockaddr) } -> std::same_as<size_t>;
    };

}  // namespace skynet::concepts
