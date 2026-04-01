cmake_minimum_required(VERSION 3.25..4.2.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(bulk_server VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  if (NOT MSVC)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      add_compile_options(-fcolor-diagnostics)
    else()
      add_compile_options(-fdiagnostics-color=always)
    endif()
  endif()

  set(MINIMUM_CXX_STANDARD 20)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if (PROJECT_IS_TOP_LEVEL)
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

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(HW_10_COMPILE_WARNING_FLAGS -Wc++20-compat-pedantic)
endif()

add_library(server_async STATIC lib/async.cpp)
target_include_directories(server_async PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(server_async PRIVATE custom_print)
target_compile_options(server_async PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_10_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(server_async PRIVATE -Wno-effc++)
endif()

add_library(server STATIC lib/server.cpp)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(server PRIVATE Boost::asio custom_print server_async)
target_compile_options(server PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_10_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(server PRIVATE -Wno-effc++ -Wno-null-dereference)
endif()

add_executable(bulk_server src/main.cpp)
target_link_libraries(bulk_server PRIVATE custom_print Microsoft.GSL::GSL server)
target_compile_options(bulk_server PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_10_COMPILE_WARNING_FLAGS})
if (MSVC)
  target_compile_definitions(server PRIVATE _WIN32_WINNT=0x0A00)
  target_compile_definitions(bulk_server PRIVATE _WIN32_WINNT=0x0A00)
else()
  target_compile_options(bulk_server PRIVATE
    -Wno-unsafe-buffer-usage-in-container -Wno-unsafe-buffer-usage)
  target_link_libraries(bulk_server PRIVATE -static)
endif()

add_executable(bulk_server_test test/bulk_server_test.cpp)
target_link_libraries(bulk_server_test PRIVATE
  capture GTest::gmock GTest::gtest_main server socket_wrapper)
target_compile_options(bulk_server_test PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_10_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(bulk_server_test PRIVATE -Wno-alloc-zero -Wno-strict-overflow)
  else()
    target_compile_options(bulk_server_test PRIVATE
      -Wno-unsafe-buffer-usage -Wno-global-constructors)
  endif()
endif()

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
