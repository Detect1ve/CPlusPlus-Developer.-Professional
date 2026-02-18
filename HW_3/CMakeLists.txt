cmake_minimum_required(VERSION 3.25..4.2.3)

project(HW_3 LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  set(CMAKE_COLOR_DIAGNOSTICS ON)

  set(MINIMUM_CXX_STANDARD 14)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(../cmake/clang-tidy.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/fetch_gsl.cmake)
  include(../cmake/gcc_analyzer.cmake)
  include(../cmake/warning_flags.cmake)
  include(../common/custom_print.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_library(list_and_allocator INTERFACE)
target_include_directories(list_and_allocator
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(list_and_allocator INTERFACE Microsoft.GSL::GSL)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE custom_print list_and_allocator)
if (NOT MSVC)
  target_link_libraries(main PRIVATE -static)
endif()
set_warning_flags(main
  EXTRA_CLANG -Wc++14-compat-pedantic)

add_executable(my_allocator_test test/my_allocator_test.cpp)
target_link_libraries(my_allocator_test PRIVATE GTest::gtest_main list_and_allocator)
set_warning_flags(my_allocator_test
  EXTRA_CLANG -Wc++14-compat-pedantic -Wno-global-constructors)

add_executable(my_list_test test/my_list_test.cpp)
target_link_libraries(my_list_test PRIVATE GTest::gtest_main list_and_allocator)
set_warning_flags(my_list_test
  EXTRA_CLANG -Wc++14-compat-pedantic -Wno-global-constructors)

set_analyzer(main
  EXCLUDE
    "-Wno-analyzer-use-of-uninitialized-value"
  EXCLUDE_MINSIZEREL
    "-Wno-analyzer-use-of-uninitialized-value")
set_analyzer(my_allocator_test
  EXCLUDE_MINSIZEREL
    "-Wno-analyzer-malloc-leak"
  EXCLUDE_RELEASE
    "-Wno-analyzer-malloc-leak")
set_analyzer(my_list_test
  EXCLUDE_MINSIZEREL
    "-Wno-analyzer-malloc-leak"
    "-Wno-analyzer-use-of-uninitialized-value"
  EXCLUDE_RELEASE
    "-Wno-analyzer-malloc-leak")

set(CLANG_TIDY_HW_3_OPTS
  "-fuchsia-overloaded-operator"
  "-modernize-use-trailing-return-type")

set_smart_tidy(main
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  EXCLUDE
    "-llvm-prefer-static-over-anonymous-namespace"
    "${CLANG_TIDY_HW_3_OPTS}"
  EXTRA_INCLUDES
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_tidy(my_allocator_test
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  EXCLUDE
    "-cert-err58-cpp"
    "-cppcoreguidelines-owning-memory"
    "-fuchsia-default-arguments-calls"
    "-fuchsia-statically-constructed-objects"
    "${CLANG_TIDY_HW_3_OPTS}"
  HAS_EXCEPTIONS)

set_smart_tidy(my_list_test
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  EXCLUDE
    "${CLANG_TIDY_HW_3_OPTS}")

enable_testing()
add_test(NAME HW_3.my_allocator_test COMMAND $<TARGET_FILE:my_allocator_test>)
add_test(NAME HW_3.my_list_test COMMAND $<TARGET_FILE:my_list_test>)
