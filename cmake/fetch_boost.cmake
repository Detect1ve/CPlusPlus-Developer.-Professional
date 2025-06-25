find_package(Boost 1.89.0 CONFIG COMPONENTS unit_test_framework algorithm uuid crc
  filesystem program_options asio)
if (NOT Boost_FOUND)
  include(FetchContent)
  set(BOOST_INCLUDE_LIBRARIES test algorithm uuid crc filesystem program_options asio)
  FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.89.0/boost-1.89.0-cmake.tar.xz
    URL_MD5 537da0e22b31b8b7185cc44fdde70458
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    EXCLUDE_FROM_ALL)
  FetchContent_MakeAvailable(Boost)
endif()
