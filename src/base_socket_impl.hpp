#pragma once

#include <skynet/base_socket.hpp>

#include <swiss/strong_typedef.hpp>

#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet {

class base_socket::impl final
    : public swiss::strong_typedef<class native_handle_tag,
                                   platform::native_handle_t> {
 public:
  using swiss::strong_typedef<native_handle_tag,
                              platform::native_handle_t>::strong_typedef;
};

}  // namespace skynet
