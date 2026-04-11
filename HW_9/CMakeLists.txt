cmake_minimum_required(VERSION 3.25..4.3.1)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(async VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  set(CMAKE_COLOR_DIAGNOSTICS ON)

  set(MINIMUM_CXX_STANDARD 20)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

  include(../cmake/clang-tidy.cmake)
  include(../cmake/cppcheck.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/warning_flags.cmake)
  include(../common/capture.cmake)
  include(../common/custom_print.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_library(async SHARED lib/async.cpp include/async.h)
target_include_directories(async PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(async PRIVATE custom_print)
set_warning_flags(async
  EXTRA_CLANG
    -Wc++20-compat-pedantic
  EXTRA_GCC
    -Wno-effc++
    -Wno-strict-overflow)
if (WIN32)
  set_target_properties(async PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_library(async_static STATIC lib/async.cpp)
target_include_directories(async_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(async_static PRIVATE custom_print)
set_warning_flags(async_static
  EXTRA_CLANG
    -Wc++20-compat-pedantic
  EXTRA_GCC
    -Wno-effc++
    -Wno-strict-overflow)

add_executable(async_cli src/main.cpp)
target_link_libraries(async_cli PRIVATE async_static)
if (NOT MSVC)
  target_link_libraries(async_cli PRIVATE -static)
endif()
set_warning_flags(async_cli
  EXTRA_CLANG
    -Wc++20-compat-pedantic)

add_executable(async_test test/async_test.cpp)
target_link_libraries(async_test PRIVATE async capture GTest::gmock GTest::gtest_main)
set_warning_flags(async_test
  EXTRA_CLANG
    -Wc++20-compat-pedantic
    -Wno-global-constructors
    -Wno-unsafe-buffer-usage)

set(CLANG_TIDY_HW_9_OPTS
  "-llvm-header-guard"
  "-llvm-namespace-comment"
  "-modernize-use-trailing-return-type"
  "-portability-avoid-pragma-once")

set_smart_tidy(async
  EXCLUDE
    "-altera-struct-pack-align"
    "-fuchsia-default-arguments-calls"
    "-llvm-prefer-static-over-anonymous-namespace"
    "${CLANG_TIDY_HW_9_OPTS}"
  EXTRA_INCLUDES
    "${CUSTOM_PRINT_INCLUDE_DIR}")

set_smart_tidy(async_static
  EXCLUDE
    "-altera-struct-pack-align"
    "-fuchsia-default-arguments-calls"
    "-llvm-prefer-static-over-anonymous-namespace"
    "${CLANG_TIDY_HW_9_OPTS}"
  EXTRA_INCLUDES
    "${CUSTOM_PRINT_INCLUDE_DIR}")

set_smart_tidy(async_cli
  EXCLUDE
    "-cppcoreguidelines-avoid-magic-numbers"
    "-hicpp-named-parameter"
    "-llvm-qualified-auto"
    "-misc-const-correctness"
    "-misc-include-cleaner"
    "-readability-identifier-length"
    "-readability-magic-numbers"
    "-readability-named-parameter"
    "-readability-qualified-auto"
    "${CLANG_TIDY_HW_9_OPTS}")

set_smart_tidy(async_test
  COGNITIVE_IGNORE_MACROS
  EXCLUDE
    "-fuchsia-default-arguments-calls"
    "-llvm-prefer-static-over-anonymous-namespace"
    "${CLANG_TIDY_HW_9_OPTS}"
  EXTRA_INCLUDES
    "${CAPTURE_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_cppcheck(async
  EXCLUDE
    "unreadVariable"
    "unusedFunction")

set_smart_cppcheck(async_static
  EXCLUDE
    "unreadVariable"
    "unusedFunction")

set_smart_cppcheck(async_cli)
set_smart_cppcheck(async_test
  LIBRARIES
    "googletest,posix")

enable_testing()
add_test(NAME HW_9.async_test COMMAND $<TARGET_FILE:async_test>)

install(TARGETS async
        LIBRARY DESTINATION lib
        COMPONENT "${PROJECT_NAME}")
install(FILES include/async.h
        DESTINATION include/async
        COMPONENT "${PROJECT_NAME}")
install(TARGETS async_cli
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
