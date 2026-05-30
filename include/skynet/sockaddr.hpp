#pragma once

#include <concepts>

#include "family.hpp"

namespace skynet::concepts {

template <typename T>
concept sockaddr = requires(T sockaddr) {
  { sockaddr.length() } -> std::same_as<std::size_t>;
  { sockaddr.family() } -> std::same_as<family>;

  requires std::equality_comparable<T>;
};

}  // namespace skynet::concepts
