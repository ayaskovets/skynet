cmake_minimum_required(VERSION 3.25.0)
project(skynet-unit-tests LANGUAGES CXX)

# ------------------------------------------------------------------------------
# dependencies
# ------------------------------------------------------------------------------

find_package(GTest REQUIRED)

if(PROJECT_IS_TOP_LEVEL)
  find_package(skynet REQUIRED)
endif()

include(CTest)
include(GoogleTest)

# ------------------------------------------------------------------------------
# general settings and options
# ------------------------------------------------------------------------------

add_executable(skynet-unit-tests)

# ------------------------------------------------------------------------------
# sources
# ------------------------------------------------------------------------------

set(SOURCES
    base_sockaddr_test.cpp
    base_socket_test.cpp
    dns_test.cpp
    inet/ip_test.cpp
    inet/port_test.cpp
    inet/sockaddr_test.cpp
    inet/tcp/socket_test.cpp
    inet/udp/socket_test.cpp
    inet6/ip_test.cpp
    inet6/port_test.cpp
    inet6/sockaddr_test.cpp
    inet6/tcp/socket_test.cpp
    inet6/udp/socket_test.cpp
    multiplexing_test.cpp
    sockaddr_test.cpp
    socket_test.cpp
    unix/dgram/socket_test.cpp
    unix/sockaddr_test.cpp
    unix/stream/socket_test.cpp)

# ------------------------------------------------------------------------------
# target
# ------------------------------------------------------------------------------

target_sources(skynet-unit-tests PRIVATE ${SOURCES})

target_link_libraries(skynet-unit-tests PRIVATE skynet::skynet
                                                GTest::gtest_main)

gtest_discover_tests(skynet-unit-tests)
