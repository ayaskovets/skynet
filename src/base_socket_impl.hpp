#pragma once

#include <skynet/base_socket.hpp>

#include "platform/platform.hpp"  // IWYU pragma: keep

namespace skynet {

class base_socket::impl final
    : public swiss::strong_typedef<class native_handle_tag,
                                   platform::native_handle> {
 public:
  using swiss::strong_typedef<native_handle_tag,
                              platform::native_handle>::strong_typedef;
};

}  // namespace skynet
