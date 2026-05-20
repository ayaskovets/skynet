#pragma once

#include <format>
#include <string>

#include <swiss/unchecked_any.hpp>

#include "family.hpp"

namespace skynet {

/**
 * @brief Sockaddr storage of any family
 * @warning Storage for the appropriate sockaddr structure is managed by the
 * base_sockaddr. Derived classes are expected to access the storage through
 * native_sockaddr() method
 */
class base_sockaddr {
 protected:
  explicit base_sockaddr(family family);

 protected:
  [[nodiscard]] auto native_sockaddr() noexcept -> void *;
  [[nodiscard]] auto native_sockaddr() const noexcept -> void const *;

 public:
  auto operator==(base_sockaddr const & that) const -> bool;
  auto operator!=(base_sockaddr const & that) const -> bool;

 public:
  /**
   * @brief Storage size in bytes
   */
  [[nodiscard]] auto length() const -> std::size_t;
  [[nodiscard]] auto family() const -> family;

 public:
  [[nodiscard]] auto to_string() const -> std::string;

 private:
  swiss::unchecked_any impl_;
};

}  // namespace skynet

template <>
class std::formatter<skynet::base_sockaddr> {
 public:
  template <typename FormatContext>
  constexpr auto parse(FormatContext & ctx) const noexcept {
    return ctx.begin();
  }

  template <typename FormatContext>
  constexpr auto format(skynet::base_sockaddr const & sockaddr,
                        FormatContext & ctx) const {
    return std::format_to(ctx.out(), "{}", sockaddr.to_string());
  }
};
