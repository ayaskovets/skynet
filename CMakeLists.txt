cmake_minimum_required(VERSION 3.25.0)
project(
  skynet
  VERSION 0.0.1
  LANGUAGES CXX)

# ------------------------------------------------------------------------------
# dependencies
# ------------------------------------------------------------------------------

find_package(swiss REQUIRED)

# ------------------------------------------------------------------------------
# general settings and options
# ------------------------------------------------------------------------------

option(BUILD_BENCHMARKS "Build the benchmarks tree." OFF)

add_library(skynet)
add_library(skynet::skynet ALIAS skynet)

# ------------------------------------------------------------------------------
# sources
# ------------------------------------------------------------------------------

include(GenerateExportHeader)

generate_export_header(skynet EXPORT_FILE_NAME
                       "${CMAKE_CURRENT_BINARY_DIR}/include/skynet/export.hpp")

set(SOURCES
    src/base_sockaddr.cpp
    src/dns.cpp
    src/inet/ip.cpp
    src/inet/port.cpp
    src/inet/sockaddr.cpp
    src/inet6/ip.cpp
    src/inet6/sockaddr.cpp)
set(HEADERS
    "${CMAKE_CURRENT_BINARY_DIR}/include/skynet/export.hpp"
    include/skynet/base_sockaddr.hpp
    include/skynet/dns.hpp
    include/skynet/family.hpp
    include/skynet/inet/ip.hpp
    include/skynet/inet/port.hpp
    include/skynet/inet/sockaddr.hpp
    include/skynet/inet6/ip.hpp
    include/skynet/inet6/port.hpp
    include/skynet/inet6/sockaddr.hpp
    include/skynet/protocol.hpp
    include/skynet/socktype.hpp)

# ------------------------------------------------------------------------------
# target
# ------------------------------------------------------------------------------

target_sources(
  skynet
  PRIVATE ${SOURCES}
  PUBLIC FILE_SET
         HEADERS
         BASE_DIRS
         "${CMAKE_CURRENT_SOURCE_DIR}/include"
         "${CMAKE_CURRENT_BINARY_DIR}/include"
         FILES
         ${HEADERS})
target_compile_features(skynet PUBLIC cxx_std_23)
set_target_properties(
  skynet
  PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}
             VERSION ${PROJECT_VERSION}
             POSITION_INDEPENDENT_CODE ON
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN ON)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(skynet PUBLIC SKYNET_STATIC_DEFINE)
endif()

target_link_libraries(skynet PUBLIC swiss::swiss)

# ------------------------------------------------------------------------------
# install
# ------------------------------------------------------------------------------

if(PROJECT_IS_TOP_LEVEL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)

  configure_package_config_file(
    cmake/skynet-config.cmake.in skynet-config.cmake
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/skynet")

  write_basic_package_version_file(skynet-config-version.cmake
                                   COMPATIBILITY SameMajorVersion)

  install(
    TARGETS skynet
    EXPORT skynet_export
    RUNTIME COMPONENT skynet
    LIBRARY COMPONENT skynet NAMELINK_COMPONENT skynet-dev
    ARCHIVE COMPONENT skynet-dev
            FILE_SET HEADERS
            COMPONENT skynet-dev)

  install(
    EXPORT skynet_export
    COMPONENT skynet-dev
    FILE "skynet-targets.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/skynet"
    NAMESPACE skynet::)

  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/skynet-config.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/skynet-config-version.cmake"
    COMPONENT skynet-dev
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/skynet")
endif()

# ------------------------------------------------------------------------------
# benchmarks
# ------------------------------------------------------------------------------

if(PROJECT_IS_TOP_LEVEL)
  if(BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
  endif()
endif()

# ------------------------------------------------------------------------------
# tests
# ------------------------------------------------------------------------------
if(PROJECT_IS_TOP_LEVEL)
  include(CTest)

  if(BUILD_TESTING)
    add_subdirectory(tests/unit)
  endif()
endif()
