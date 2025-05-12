cmake_minimum_required(VERSION 4.0.3)

project(bayan LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
  add_compile_options(-fdiagnostics-color=always)
  include(cmake/googletest.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_executable(bayan src/main.cpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost 1.88.0 CONFIG COMPONENTS crc program_options uuid algorithm filesystem)
if(NOT Boost_FOUND)
  include(FetchContent)
  set(BOOST_INCLUDE_LIBRARIES crc program_options uuid algorithm filesystem)
  FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.88.0/boost-1.88.0-cmake.tar.xz
    URL_MD5 3edffaacd2cfe63c240ef1b99497c74f
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    EXCLUDE_FROM_ALL)
  FetchContent_MakeAvailable(Boost)
endif()

target_link_libraries(bayan PRIVATE Boost::crc Boost::program_options Boost::uuid
  Boost::algorithm Boost::filesystem -static)

set_target_properties(bayan PROPERTIES CXX_STANDARD 26)

add_executable(bayan_test test/bayan.cpp)
