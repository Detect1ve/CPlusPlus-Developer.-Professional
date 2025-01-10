find_package(Boost 1.88.0 CONFIG COMPONENTS unit_test_framework)
if(NOT Boost_FOUND)
  include(FetchContent)
  set(BOOST_INCLUDE_LIBRARIES test)
  FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.88.0/boost-1.88.0-cmake.tar.xz
    URL_MD5 3edffaacd2cfe63c240ef1b99497c74f
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    EXCLUDE_FROM_ALL)
  FetchContent_MakeAvailable(Boost)
endif()
