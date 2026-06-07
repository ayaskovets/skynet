cmake_minimum_required(VERSION 3.25.0)
project(skynet-benchmarks LANGUAGES CXX)

# ------------------------------------------------------------------------------
# dependencies
# ------------------------------------------------------------------------------

find_package(benchmark REQUIRED)

if(PROJECT_IS_TOP_LEVEL)
  find_package(skynet REQUIRED)
endif()

include(CTest)
include(GoogleTest)

# ------------------------------------------------------------------------------
# general settings and options
# ------------------------------------------------------------------------------

add_executable(skynet-benchmarks)

# ------------------------------------------------------------------------------
# sources
# ------------------------------------------------------------------------------

set(SOURCES
    inet/tcp/socket_bench.cpp
    inet/udp/socket_bench.cpp
    inet/sockaddr_bench.cpp
    inet6/tcp/socket_bench.cpp
    inet6/udp/socket_bench.cpp
    inet6/sockaddr_bench.cpp
    unix/stream/socket_bench.cpp
    unix/dgram/socket_bench.cpp
    unix/sockaddr_bench.cpp
    unix/sockaddr_bench.cpp
    dns_bench.cpp
    main.cpp)

# ------------------------------------------------------------------------------
# target
# ------------------------------------------------------------------------------

target_sources(skynet-benchmarks PRIVATE ${SOURCES})

target_link_libraries(skynet-benchmarks PRIVATE skynet::skynet
                                                benchmark::benchmark)

target_include_directories(skynet-benchmarks PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# ------------------------------------------------------------------------------
# workflow automation
# ------------------------------------------------------------------------------

add_test(NAME skynet-benchmarks COMMAND skynet-benchmarks
                                        --benchmark_color=true)
set_tests_properties(skynet-benchmarks PROPERTIES LABELS benchmark)
