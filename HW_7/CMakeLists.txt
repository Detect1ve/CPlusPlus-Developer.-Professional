cmake_minimum_required(VERSION 3.25..4.2.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(bulk VERSION ${PROJECT_VERSION} LANGUAGES CXX)

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
  include(../cmake/lto_enable.cmake)
  include(../cmake/warning_flags.cmake)
  include(../common/attribute_wrapper.cmake)
  include(../common/capture.cmake)
  include(../common/custom_print.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_library(taskmanager STATIC lib/taskmanager.cpp)
target_include_directories(taskmanager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(taskmanager
  PRIVATE custom_print
  PUBLIC attribute_wrapper)
set_warning_flags(taskmanager
  EXTRA_CLANG -Wc++20-compat-pedantic
  EXTRA_GCC -Wno-strict-overflow)

set_warning_flags(taskmanager
  EXTRA_GCC -Wno-effc++
  NO_BASE
  SCOPE "PUBLIC")

add_executable(bulk src/main.cpp)
target_link_libraries(bulk PRIVATE custom_print taskmanager)
set_warning_flags(bulk
  EXTRA_CLANG -Wc++20-compat-pedantic -Wno-unsafe-buffer-usage)
if (NOT MSVC)
  target_link_libraries(bulk PRIVATE -static)
endif()

add_executable(taskmanager_test test/taskmanager_test.cpp)
target_link_libraries(taskmanager_test PRIVATE
  capture custom_print GTest::gtest_main taskmanager)
set_warning_flags(taskmanager_test
  EXTRA_CLANG -Wc++20-compat-pedantic -Wno-global-constructors -Wno-unsafe-buffer-usage)

set_analyzer(taskmanager
  EXCLUDE
    "-Wno-analyzer-use-of-uninitialized-value"
  EXCLUDE_MINSIZEREL
    "-Wno-analyzer-use-of-uninitialized-value")
set_analyzer(bulk
  EXCLUDE
    "-Wno-analyzer-use-of-uninitialized-value"
  EXCLUDE_MINSIZEREL
    "-Wno-analyzer-use-of-uninitialized-value")
set_analyzer(taskmanager_test
  EXCLUDE
    "-Wno-analyzer-use-of-uninitialized-value"
  EXCLUDE_MINSIZEREL
    "-Wno-analyzer-malloc-leak"
    "-Wno-analyzer-use-of-uninitialized-value")

enable_lto(bulk taskmanager taskmanager_test)

set(CLANG_TIDY_HW_7_OPTS
  "-fuchsia-default-arguments-calls"
  "-modernize-use-trailing-return-type")

set_smart_tidy(taskmanager
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  EXCLUDE
    "-fuchsia-overloaded-operator"
    "-llvm-prefer-static-over-anonymous-namespace"
    "${CLANG_TIDY_HW_7_OPTS}"
  EXTRA_INCLUDES
    "${ATTRIBUTE_WRAPPER_INCLUDE_DIR}"
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_tidy(bulk
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  EXCLUDE
    "${CLANG_TIDY_HW_7_OPTS}"
  EXTRA_INCLUDES
    "${ATTRIBUTE_WRAPPER_INCLUDE_DIR}"
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_tidy(taskmanager_test
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  EXCLUDE
    "-llvm-prefer-static-over-anonymous-namespace"
    "${CLANG_TIDY_HW_7_OPTS}"
  EXTRA_INCLUDES
    "${ATTRIBUTE_WRAPPER_INCLUDE_DIR}"
    "${CAPTURE_INCLUDE_DIR}"
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

enable_testing()
add_test(NAME HW_7.taskmanager_test COMMAND $<TARGET_FILE:taskmanager_test>)

install(TARGETS bulk RUNTIME DESTINATION bin
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
