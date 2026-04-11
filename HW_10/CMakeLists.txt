cmake_minimum_required(VERSION 3.25..4.3.1)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(bulk_server VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  set(CMAKE_COLOR_DIAGNOSTICS ON)

  set(MINIMUM_CXX_STANDARD 20)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

  include(../cmake/clang-tidy.cmake)
  include(../cmake/cppcheck.cmake)
  include(../cmake/fetch_boost.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/warning_flags.cmake)
  include(../common/capture.cmake)
  include(../common/custom_print.cmake)
  include(../common/socket_wrapper.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_library(server_async STATIC lib/async.cpp)
target_include_directories(server_async PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(server_async PRIVATE custom_print)
set_warning_flags(server_async
  EXTRA_CLANG
    -Wc++20-compat-pedantic
  EXTRA_GCC
    -Wno-effc++)

add_library(server STATIC lib/server.cpp)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(server PRIVATE Boost::asio custom_print server_async)
set_warning_flags(server
  EXTRA_CLANG
    -Wc++20-compat-pedantic
  EXTRA_GCC
    -Wno-effc++
    -Wno-null-dereference)

add_executable(bulk_server src/main.cpp)
target_link_libraries(bulk_server PRIVATE custom_print Microsoft.GSL::GSL server)
set_warning_flags(bulk_server
  EXTRA_CLANG
    -Wc++20-compat-pedantic
    -Wno-unsafe-buffer-usage)
if (MSVC)
  target_compile_definitions(server PRIVATE _WIN32_WINNT=0x0A00)
  target_compile_definitions(bulk_server PRIVATE _WIN32_WINNT=0x0A00)
else()
  target_link_libraries(bulk_server PRIVATE -static)
endif()

add_executable(bulk_server_test test/bulk_server_test.cpp)
target_link_libraries(bulk_server_test PRIVATE
  capture GTest::gmock GTest::gtest_main server socket_wrapper)
set_warning_flags(bulk_server_test
  EXTRA_CLANG
    -Wc++20-compat-pedantic
    -Wno-global-constructors
    -Wno-unsafe-buffer-usage
  EXTRA_GCC
    -Wno-alloc-zero
    -Wno-strict-overflow)

set(CLANG_TIDY_HW_10_OPTS
    "-llvm-header-guard"
    "-llvm-namespace-comment"
    "-modernize-use-trailing-return-type"
    "-portability-avoid-pragma-once")

set_smart_tidy(server_async
  EXCLUDE
    "-altera-struct-pack-align"
    "-fuchsia-default-arguments-calls"
    "-llvm-prefer-static-over-anonymous-namespace"
    "${CLANG_TIDY_HW_10_OPTS}"
  EXTRA_INCLUDES
    "${CUSTOM_PRINT_INCLUDE_DIR}")

set_smart_tidy(server
  EXCLUDE
    "-fuchsia-default-arguments-calls"
    "-llvm-prefer-static-over-anonymous-namespace"
    "-misc-no-recursion"
    "${CLANG_TIDY_HW_10_OPTS}"
  EXTRA_INCLUDES
    "${CUSTOM_PRINT_INCLUDE_DIR}")

set_smart_tidy(bulk_server
  EXCLUDE
    "${CLANG_TIDY_HW_10_OPTS}"
  EXTRA_INCLUDES
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_tidy(bulk_server_test
  COGNITIVE_IGNORE_MACROS
  EXCLUDE
    "-fuchsia-default-arguments-calls"
    "-llvm-prefer-static-over-anonymous-namespace"
    "${CLANG_TIDY_HW_10_OPTS}"
  EXTRA_INCLUDES
    "${CAPTURE_INCLUDE_DIR}"
    "${SOCKET_WRAPPER_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_cppcheck(server_async
  EXCLUDE
    "unreadVariable"
    "unusedFunction")

set_smart_cppcheck(server
  EXCLUDE
    "unusedFunction"
  LIBRARIES
    "boost,posix")

set_smart_cppcheck(bulk_server
  EXCLUDE
    "unmatchedSuppression"
    "unusedFunction")

set_smart_cppcheck(bulk_server_test
  EXCLUDE
    "unusedFunction"
  LIBRARIES
    "googletest,posix")

enable_testing()
add_test(NAME HW_10.bulk_server_test COMMAND $<TARGET_FILE:bulk_server_test>)

install(TARGETS bulk_server
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
