#pragma once

#include <chrono>

#include <swiss/strong_typedef.hpp>

namespace skynet {

using acceptconn = swiss::strong_typedef<class acceptconn_t, bool>;
using broadcast  = swiss::strong_typedef<class broadcast_t, bool>;
using keepalive  = swiss::strong_typedef<class keepalive_t, bool>;
using linger     = swiss::strong_typedef<class linger_t, std::chrono::seconds>;
using nonblock   = swiss::strong_typedef<class nonblock_t, bool>;
using oobinline  = swiss::strong_typedef<class oobinline_t, bool>;
using rcvbuf     = swiss::strong_typedef<class rcvbuf_t, std::size_t>;
using reuseaddr  = swiss::strong_typedef<class reuseaddr_t, bool>;
using reuseport  = swiss::strong_typedef<class reuseport_t, bool>;
using sndbuf     = swiss::strong_typedef<class sndbuf_t, std::size_t>;
using nodelay    = swiss::strong_typedef<class nodelay_t, bool>;

}  // namespace skynet
