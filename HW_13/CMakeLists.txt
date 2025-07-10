cmake_minimum_required(VERSION 4.0.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(fashio_mnist VERSION ${PROJECT_VERSION} LANGUAGES CXX)

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
    include(../googletest.cmake)
endif()

find_package(Eigen3 3.4.0)
if (NOT Eigen_FOUND)
  include(FetchContent)
  set(BUILD_TESTING OFF CACHE BOOL "Disable Eigen tests" FORCE)
  FetchContent_Declare(
    Eigen3
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL)
  FetchContent_MakeAvailable(Eigen3)
endif()

add_library(mlp_lib STATIC lib/mlp.cpp)
target_link_libraries(mlp_lib PUBLIC Eigen3::Eigen)
target_include_directories(mlp_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(fashio_mnist src/main.cpp)
target_link_libraries(fashio_mnist mlp_lib -static)

enable_testing()
add_executable(fashio_mnist_test test/mlp_test.cpp)
target_link_libraries(fashio_mnist_test mlp_lib GTest::gtest_main)

target_compile_definitions(fashio_mnist_test PRIVATE
  PROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_test(HW_13.fashio_mnist_test fashio_mnist_test)

install(TARGETS fashio_mnist
        RUNTIME DESTINATION bin
        COMPONENT "${PROJECT_NAME}")
if (PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_13_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
