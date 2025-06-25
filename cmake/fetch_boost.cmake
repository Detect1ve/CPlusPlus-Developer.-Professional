find_package(Boost 1.90.0 CONFIG COMPONENTS unit_test_framework algorithm uuid crc
  filesystem program_options asio)
if (NOT Boost_FOUND)
  include(FetchContent)
  set(BOOST_INCLUDE_LIBRARIES test algorithm uuid crc filesystem program_options asio)
  FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.90.0/boost-1.90.0-cmake.tar.xz
    URL_HASH SHA256=aca59f889f0f32028ad88ba6764582b63c916ce5f77b31289ad19421a96c555f
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    EXCLUDE_FROM_ALL
    SYSTEM)
  FetchContent_MakeAvailable(Boost)
endif()
