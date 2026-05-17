cmake_minimum_required(VERSION 3.25..4.3.1)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(bayan VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  set(CMAKE_COLOR_DIAGNOSTICS ON)

  set(MINIMUM_CXX_STANDARD 23)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

  include(../cmake/clang-tidy.cmake)
  include(../cmake/cppcheck.cmake)
  include(../cmake/fetch_boost.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/warning_flags.cmake)
  include(../common/attribute_wrapper.cmake)
  include(../common/capture.cmake)
  include(../common/custom_print.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_library(bayan_lib STATIC lib/bayan.cpp)
target_include_directories(bayan_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(bayan_lib PRIVATE
  attribute_wrapper Boost::algorithm Boost::crc Boost::filesystem Boost::program_options
  Boost::uuid custom_print)
set_warning_flags(bayan_lib
  EXTRA_CLANG
    -Wc++20-compat-pedantic
  EXTRA_GCC
    -Wno-alloc-zero
    -Wno-effc++
    -Wno-null-dereference
    -Wno-strict-overflow)

add_executable(bayan src/main.cpp)
target_link_libraries(bayan PRIVATE bayan_lib custom_print)
if (NOT MSVC)
  target_link_libraries(bayan PRIVATE -static)
endif()
set_warning_flags(bayan
  EXTRA_CLANG
    -Wc++20-compat-pedantic
    -Wno-unsafe-buffer-usage-in-container)

add_executable(bayan_test test/bayan_test.cpp)
target_link_libraries(bayan_test PRIVATE
  bayan_lib Boost::filesystem capture GTest::gtest_main)
set_warning_flags(bayan_test
  EXTRA_CLANG
    -Wc++20-compat-pedantic
    -Wno-global-constructors)

set(CLANG_TIDY_HW_8_OPTS
    "-altera-struct-pack-align"
    "-fuchsia-default-arguments-calls")

set_smart_tidy(bayan_lib
  EXCLUDE
    "${CLANG_TIDY_HW_8_OPTS}"
  EXTRA_TARGETS
    attribute_wrapper
    custom_print
  HAS_EXCEPTIONS)

set_smart_tidy(bayan
  EXCLUDE
    "${CLANG_TIDY_HW_8_OPTS}"
  EXTRA_TARGETS
    custom_print
  HAS_EXCEPTIONS)

set_smart_tidy(bayan_test
  COGNITIVE_IGNORE_MACROS
  EXCLUDE
    "${CLANG_TIDY_HW_8_OPTS}"
  EXTRA_TARGETS
    capture
  HAS_EXCEPTIONS)

set_smart_cppcheck(bayan_lib
  EXCLUDE
    "missingMemberCopy"
    "unusedFunction"
  LIBRARIES
    "boost,gnu")

set_smart_cppcheck(bayan
  EXCLUDE
    "unmatchedSuppression"
    "unusedFunction")

set_smart_cppcheck(bayan_test
  EXCLUDE
    "unreadVariable" # TODO
  LIBRARIES
    "boost,googletest,posix")

enable_testing()
add_test(NAME HW_8.bayan_test COMMAND $<TARGET_FILE:bayan_test>)

install(TARGETS bayan RUNTIME DESTINATION bin
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
