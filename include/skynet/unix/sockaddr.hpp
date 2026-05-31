#pragma once

#include <skynet/base_sockaddr.hpp>

#include <filesystem>

namespace skynet::unix {

/**
 * @brief UNIX address
 */
class sockaddr final : public base_sockaddr {
 public:
  static auto kInvalid() noexcept -> sockaddr const &;
  static auto kEmpty() noexcept -> sockaddr const &;

 public:
  explicit sockaddr(std::filesystem::path const & path);

 public:
  [[nodiscard]] auto get_path() const noexcept -> std::filesystem::path;
};

}  // namespace skynet::unix

template <>
class std::formatter<skynet::unix::sockaddr> {
 public:
  template <typename FormatContext>
  constexpr auto parse(FormatContext & ctx) const noexcept {
    return ctx.begin();
  }

  template <typename FormatContext>
  constexpr auto format(skynet::unix::sockaddr const & sockaddr,
                        FormatContext & ctx) const {
    return std::format_to(ctx.out(), "{}", sockaddr.to_string());
  }
};
