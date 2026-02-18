cmake_minimum_required(VERSION 3.25..4.2.3)

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
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(../cmake/clang-tidy.cmake)
  include(../cmake/fetch_absl.cmake)
  include(../cmake/fetch_boost.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/gcc_analyzer.cmake)
  include(../cmake/lto_enable.cmake)
  include(../cmake/warning_flags.cmake)
  include(../common/attribute_wrapper.cmake)
  include(../common/capture.cmake)
  include(../common/custom_print.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_library(bayan_lib STATIC lib/bayan.cpp)
target_include_directories(bayan_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(bayan_lib PRIVATE
  attribute_wrapper Boost::crc Boost::filesystem Boost::hash2 Boost::program_options
  custom_print)
set_warning_flags(bayan_lib
  EXTRA_CLANG -Wc++20-compat-pedantic
  EXTRA_GCC -Wno-effc++)

add_executable(bayan src/main.cpp)
target_link_libraries(bayan PRIVATE bayan_lib custom_print)
set_warning_flags(bayan
  EXTRA_CLANG -Wc++20-compat-pedantic -Wno-unsafe-buffer-usage-in-container)
if (NOT MSVC)
  target_link_libraries(bayan PRIVATE -static)
endif()

add_executable(bayan_test test/bayan_test.cpp)
target_link_libraries(bayan_test PRIVATE
  absl::strings bayan_lib Boost::filesystem capture GTest::gtest_main)
set_warning_flags(bayan_test
  EXTRA_CLANG -Wc++20-compat-pedantic -Wno-global-constructors)

cmake_host_system_information(RESULT total_ram_mb QUERY TOTAL_PHYSICAL_MEMORY)

set(LIGHT_CONFIGS "Release" "MinSizeRel")

if (total_ram_mb GREATER 15992 OR CMAKE_BUILD_TYPE IN_LIST LIGHT_CONFIGS)
  set_analyzer(bayan_lib)
else()
  message(WARNING "Not enough RAM (${total_ram_mb} MB) for GCC static analyzer for "
    "bayan_lib target. Increase RAM or switch to Release or MinSizeRel build type.")
endif()

set_analyzer(bayan
  EXCLUDE
    "-Wno-analyzer-use-of-uninitialized-value"
  EXCLUDE_MINSIZEREL
    "-Wno-analyzer-use-of-uninitialized-value")
set_analyzer(bayan_test
  EXCLUDE_MINSIZEREL
    "-Wno-analyzer-malloc-leak")

enable_lto(bayan bayan_lib bayan_test)

set(CLANG_TIDY_HW_8_OPTS
  "-fuchsia-default-arguments-calls"
  "-modernize-use-trailing-return-type")

set_smart_tidy(bayan_lib
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  EXCLUDE
    "${CLANG_TIDY_HW_8_OPTS}"
  EXTRA_INCLUDES
    "${ATTRIBUTE_WRAPPER_INCLUDE_DIR}"
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_tidy(bayan
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  EXCLUDE
    "${CLANG_TIDY_HW_8_OPTS}"
  EXTRA_INCLUDES
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_tidy(bayan_test
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  COGNITIVE_IGNORE_MACROS
  EXCLUDE
    "${CLANG_TIDY_HW_8_OPTS}"
  EXTRA_INCLUDES
    "${CAPTURE_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

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
