cmake_minimum_required(VERSION 3.25..4.3.1)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(join_server VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  set(CMAKE_COLOR_DIAGNOSTICS ON)

  set(MINIMUM_CXX_STANDARD 20)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

  include(../cmake/clang-tidy.cmake)
  include(../cmake/cppcheck.cmake)
  include(../cmake/fetch_boost.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/fetch_gsl.cmake)
  include(../cmake/warning_flags.cmake)
  include(../common/custom_print.cmake)
  include(../common/socket_wrapper.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_library(server_and_database STATIC lib/server.cpp lib/database.cpp)
target_include_directories(server_and_database PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(server_and_database PRIVATE Boost::asio custom_print)
set_warning_flags(server_and_database
  EXTRA_CLANG
    -Wc++20-compat-pedantic
    -Wno-unsafe-buffer-usage
  EXTRA_GCC
    -Wno-effc++
    -Wno-null-dereference
    -Wno-strict-overflow)
if (MSVC)
  target_compile_definitions(server_and_database PRIVATE _WIN32_WINNT=0x0A00)
endif()

add_executable(join_server src/main.cpp)
target_link_libraries(join_server PRIVATE
  custom_print Microsoft.GSL::GSL server_and_database)
set_warning_flags(join_server
  EXTRA_CLANG
    -Wc++20-compat-pedantic
    -Wno-unsafe-buffer-usage)
if (NOT MSVC)
  target_link_libraries(join_server PRIVATE -static)
endif()

add_executable(join_server_test test/join_server_test.cpp)
target_link_libraries(join_server_test PRIVATE
  GTest::gtest_main server_and_database socket_wrapper)
set_warning_flags(join_server_test
  EXTRA_CLANG
    -Wc++20-compat-pedantic
    -Wno-global-constructors
  EXTRA_GCC
    -Wno-effc++)

set_smart_tidy(server_and_database
  EXCLUDE
    "-altera-struct-pack-align"
    "-fuchsia-default-arguments-calls"
    "-llvm-prefer-static-over-anonymous-namespace"
    "-misc-no-recursion"
  EXTRA_TARGETS
    custom_print)

set_smart_tidy(join_server
  EXTRA_TARGETS
    custom_print
  HAS_EXCEPTIONS)

set_smart_tidy(join_server_test
  EXCLUDE
    "-fuchsia-default-arguments-calls"
  EXTRA_TARGETS
    socket_wrapper
  HAS_EXCEPTIONS)

set_smart_cppcheck(server_and_database
  EXCLUDE
    "unusedFunction"
  LIBRARIES
    "boost,posix")

set_smart_cppcheck(join_server
  EXCLUDE
    "unmatchedSuppression"
    "unusedFunction")

set_smart_cppcheck(join_server_test
  LIBRARIES
    "googletest,posix")

enable_testing()
add_test(NAME HW_11.join_server_test COMMAND $<TARGET_FILE:join_server_test>)

install(TARGETS join_server
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
