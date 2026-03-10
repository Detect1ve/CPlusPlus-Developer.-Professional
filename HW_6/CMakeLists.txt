cmake_minimum_required(VERSION 3.25..4.2.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(matrix VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  set(CMAKE_COLOR_DIAGNOSTICS ON)

  set(MINIMUM_CXX_STANDARD 20)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(../cmake/clang-tidy.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/gcc_analyzer.cmake)
  include(../cmake/warning_flags.cmake)
  include(../common/capture.cmake)
  include(../common/custom_print.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_executable(matrix src/main.cpp)
target_include_directories(matrix PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(matrix PRIVATE custom_print)
set_warning_flags(matrix
  EXTRA_CLANG -Wc++20-compat-pedantic)
if (NOT MSVC)
  target_link_libraries(matrix PRIVATE -static)
endif()

add_executable(matrix_test test/matrix_test.cpp)
target_include_directories(matrix_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(matrix_test PRIVATE capture GTest::gtest_main)
set_warning_flags(matrix_test
  EXTRA_CLANG -Wc++20-compat-pedantic -Wno-global-constructors)

set_analyzer(matrix
  EXCLUDE
    "-Wno-analyzer-use-of-uninitialized-value"
  EXCLUDE_MINSIZEREL
    "-Wno-analyzer-use-of-uninitialized-value")
set_analyzer(matrix_test
  EXCLUDE
    "-Wno-analyzer-malloc-leak"
  EXCLUDE_MINSIZEREL
    "-Wno-analyzer-malloc-leak"
    "-Wno-analyzer-use-of-uninitialized-value"
  EXCLUDE_RELEASE
    "-Wno-analyzer-malloc-leak"
    "-Wno-analyzer-use-of-uninitialized-value")

set(CLANG_TIDY_HW_6_OPTS
  "-fuchsia-overloaded-operator"
  "-modernize-use-trailing-return-type")

set_smart_tidy(matrix
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  EXCLUDE
    "${CLANG_TIDY_HW_6_OPTS}"
  EXTRA_INCLUDES
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_tidy(matrix_test
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  COGNITIVE_IGNORE_MACROS
  EXCLUDE
    "-cppcoreguidelines-avoid-magic-numbers"
    "-fuchsia-default-arguments-calls"
    "-performance-avoid-endl"
    "-readability-identifier-length"
    "-readability-magic-numbers"
    "${CLANG_TIDY_HW_6_OPTS}"
  EXTRA_INCLUDES
    "${CAPTURE_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

enable_testing()
add_test(NAME HW_6.matrix_test COMMAND $<TARGET_FILE:matrix_test>)

install(TARGETS matrix RUNTIME DESTINATION bin
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
