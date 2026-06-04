#pragma once

#include <chrono>
#include <optional>

#include <swiss/strong_typedef.hpp>

namespace skynet {

using broadcast = swiss::strong_typedef<class broadcast_tag, bool>;
using keepalive = swiss::strong_typedef<class keepalive_tag, bool>;
using linger    = swiss::strong_typedef<class linger_tag,
                                        std::optional<std::chrono::seconds>>;
using nonblock  = swiss::strong_typedef<class nonblock_tag, bool>;
using oobinline = swiss::strong_typedef<class oobinline_tag, bool>;
using rcvbuf    = swiss::strong_typedef<class rcvbuf_tag, std::uint32_t>;
using reuseaddr = swiss::strong_typedef<class reuseaddr_tag, bool>;
using reuseport = swiss::strong_typedef<class reuseport_tag, bool>;
using sndbuf    = swiss::strong_typedef<class sndbuf_tag, std::uint32_t>;
using nodelay   = swiss::strong_typedef<class nodelay_tag, bool>;

}  // namespace skynet
