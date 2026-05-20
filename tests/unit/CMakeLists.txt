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

set(SOURCES dns_test.cpp)

# ------------------------------------------------------------------------------
# target
# ------------------------------------------------------------------------------

target_sources(skynet-unit-tests PRIVATE ${SOURCES})

target_link_libraries(skynet-unit-tests PRIVATE skynet::skynet
                                                GTest::gtest_main)

gtest_discover_tests(skynet-unit-tests)
