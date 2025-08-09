cmake_minimum_required(VERSION 3.25..4.1.0)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(fashio_mnist VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  if (NOT MSVC)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      add_compile_options(-fcolor-diagnostics)
    else()
      add_compile_options(-fdiagnostics-color=always)
    endif()
  endif()

  set(MINIMUM_CXX_STANDARD 14)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

if (PROJECT_IS_TOP_LEVEL)
    include(../cmake/clang-tidy.cmake)
    include(../cmake/googletest.cmake)
    list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

find_package(Eigen3 3.4.0 QUIET)
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
target_link_libraries(mlp_lib PRIVATE Eigen3::Eigen)
target_include_directories(mlp_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
if (MSVC)
  target_compile_options(mlp_lib PRIVATE /W4)
else()
  target_compile_options(mlp_lib PRIVATE -Wall -Wextra -pedantic)
endif()

add_executable(fashio_mnist src/main.cpp)
target_link_libraries(fashio_mnist mlp_lib)
if (MSVC)
  target_compile_options(fashio_mnist PRIVATE /W4)
else()
  target_link_libraries(fashio_mnist -static)
  target_compile_options(fashio_mnist PRIVATE -Wall -Wextra -pedantic)
endif()

add_executable(fashio_mnist_test test/mlp_test.cpp)
target_link_libraries(fashio_mnist_test mlp_lib GTest::gtest_main)
if (MSVC)
  target_compile_options(fashio_mnist_test PRIVATE /W4)
else()
  target_compile_options(fashio_mnist_test PRIVATE -Wall -Wextra -pedantic)
endif()

target_compile_definitions(fashio_mnist_test PRIVATE
  PROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set(CLANG_TIDY_HW_13_OPTS
    "-fuchsia-default-arguments-calls,\
     -llvmlibc-callee-namespace,\
     -llvmlibc-restrict-system-libc-headers,\
     -misc-include-cleaner,\
     -modernize-use-trailing-return-type;--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(mlp_lib PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-id-dependent-backward-branch,\
      -altera-unroll-loops,\
      -cppcoreguidelines-avoid-magic-numbers,\
      -llvmlibc-implementation-in-namespace,\
      -readability-magic-numbers,\
      ${CLANG_TIDY_HW_13_OPTS}")

  set_target_properties(fashio_mnist PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -llvmlibc-implementation-in-namespace,\
      ${CLANG_TIDY_HW_13_OPTS}")

  set_target_properties(fashio_mnist_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      ${CLANG_TIDY_HW_13_OPTS}")
endif()

enable_testing()
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
