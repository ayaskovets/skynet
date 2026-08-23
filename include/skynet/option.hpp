#pragma once

#include <swiss/concepts.hpp>

#include "options/options.hpp"

namespace skynet::concepts {

template <typename T>
concept option =
    swiss::one_of<T, options::broadcast, options::keepalive, options::linger,
                  options::nodelay, options::nonblock, options::oobinline,
                  options::rcvbuf, options::reuseaddr, options::reuseport,
                  options::sndbuf>;

}  // namespace skynet::concepts
