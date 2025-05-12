cmake_minimum_required(VERSION 4.0.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(bayan VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  if (MSVC)
    set(CMAKE_CXX_STANDARD 23)
  else()
    set(CMAKE_CXX_STANDARD 26)
    add_compile_options(-fdiagnostics-color=always)
  endif()

  set(CMAKE_CXX_STANDARD_REQUIRED True)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(../boost.cmake)
  include(../googletest.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_library(bayan_lib STATIC lib/bayan.cpp)
target_include_directories(bayan_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(bayan_lib
  PRIVATE Boost::crc Boost::program_options Boost::uuid Boost::algorithm
  PUBLIC Boost::filesystem)

add_executable(bayan src/main.cpp)
target_link_libraries(bayan PRIVATE bayan_lib)
if (NOT MSVC)
  target_link_libraries(bayan PRIVATE -static)
endif()

add_executable(bayan_test test/bayan_test.cpp)
target_link_libraries(bayan_test PRIVATE bayan_lib GTest::gtest_main)

enable_testing()
add_test(HW_8.bayan_test bayan_test)

install(TARGETS bayan RUNTIME DESTINATION bin
                      COMPONENT "${PROJECT_NAME}")
if (PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_8_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
