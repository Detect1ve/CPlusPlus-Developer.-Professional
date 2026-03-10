cmake_minimum_required(VERSION 3.25..4.2.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(fashio_mnist VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  set(CMAKE_COLOR_DIAGNOSTICS ON)

  set(MINIMUM_CXX_STANDARD 17)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(../cmake/clang-tidy.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/gcc_analyzer.cmake)
  include(../cmake/lto_enable.cmake)
  include(../cmake/warning_flags.cmake)
  include(../common/custom_print.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

include(../cmake/fetch_eigen3.cmake)

add_library(mlp_lib STATIC lib/mlp.cpp)
target_link_libraries(mlp_lib PRIVATE custom_print Eigen3::Eigen)
target_include_directories(mlp_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_warning_flags(mlp_lib
  EXTRA_CLANG -Wc++17-compat-pedantic -Wno-unsafe-buffer-usage
  EXTRA_GCC -Wno-effc++)

add_executable(fashio_mnist src/main.cpp)
target_link_libraries(fashio_mnist PRIVATE custom_print mlp_lib)
set_warning_flags(fashio_mnist
  EXTRA_CLANG -Wc++17-compat-pedantic -Wno-unsafe-buffer-usage-in-container)
if (NOT MSVC)
  target_link_libraries(fashio_mnist PRIVATE -static)
endif()

add_executable(fashio_mnist_test test/mlp_test.cpp)
target_link_libraries(fashio_mnist_test mlp_lib GTest::gtest_main)
set_warning_flags(fashio_mnist_test
  EXTRA_CLANG -Wc++17-compat-pedantic -Wno-global-constructors)

target_compile_definitions(fashio_mnist_test PRIVATE
  PROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

set_analyzer(mlp_lib
  EXCLUDE
    "-Wno-analyzer-use-of-uninitialized-value"
  EXCLUDE_MINSIZEREL
    "-Wno-analyzer-use-of-uninitialized-value")
set_analyzer(fashio_mnist
  EXCLUDE
    "-Wno-analyzer-use-of-uninitialized-value"
  EXCLUDE_MINSIZEREL
    "-Wno-analyzer-use-of-uninitialized-value")
set_analyzer(fashio_mnist_test
  EXCLUDE_MINSIZEREL
    "-Wno-analyzer-malloc-leak")

enable_lto(fashio_mnist fashio_mnist_test mlp_lib)

set(CLANG_TIDY_HW_13_OPTS
  "-fuchsia-default-arguments-calls"
  "-modernize-use-trailing-return-type")

set_smart_tidy(mlp_lib
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  EXCLUDE
    "-cppcoreguidelines-avoid-magic-numbers"
    "-llvm-prefer-static-over-anonymous-namespace"
    "-readability-magic-numbers"
    "${CLANG_TIDY_HW_13_OPTS}"
  EXTRA_INCLUDES
    "${CUSTOM_PRINT_INCLUDE_DIR}")

set_smart_tidy(fashio_mnist
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  EXCLUDE
    "-llvm-prefer-static-over-anonymous-namespace"
    "${CLANG_TIDY_HW_13_OPTS}"
  EXTRA_INCLUDES
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_tidy(fashio_mnist_test
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  EXCLUDE
    "${CLANG_TIDY_HW_13_OPTS}")

enable_testing()
add_test(NAME HW_13.fashio_mnist_test COMMAND $<TARGET_FILE:fashio_mnist_test>)

install(TARGETS fashio_mnist
        RUNTIME DESTINATION bin
        COMPONENT "${PROJECT_NAME}")
if (PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPPER)

  set(CPACK_DEBIAN_${PROJECT_NAME_UPPER}_FILE_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)

  set(CPACK_DEBIAN_${PROJECT_NAME_UPPER}_PACKAGE_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
