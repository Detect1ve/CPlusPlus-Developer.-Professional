cmake_minimum_required(VERSION 3.25..4.2.3)
project(ProducerConsumerQueue LANGUAGES CXX)

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
  include(../common/custom_print.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

# include(../cmake/fetch_google_benchmark.cmake)

add_library(data_queue INTERFACE)
target_include_directories(data_queue INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if (NOT MSVC)
  target_compile_options(data_queue INTERFACE -Wno-effc++ -Wno-strict-overflow)
endif()

add_executable(queue_test test/queue_test.cpp)
target_link_libraries(queue_test PRIVATE data_queue GTest::gtest_main)
set_warning_flags(queue_test
  EXTRA_CLANG -Wc++20-compat-pedantic -Wno-global-constructors)

enable_testing()
add_test(NAME Project_work.queue_test COMMAND $<TARGET_FILE:queue_test>)

add_executable(queue_performance_test test/queue_performance_test.cpp)
target_link_libraries(queue_performance_test PRIVATE custom_print data_queue)
set_warning_flags(queue_performance_test
  EXTRA_CLANG -Wc++20-compat-pedantic)

set_analyzer(queue_test)
set_analyzer(queue_performance_test)

set(CLANG_TIDY_PROJECT_WORK_OPTS
  "-fuchsia-default-arguments-calls"
  "-fuchsia-default-arguments-declarations"
  "-fuchsia-overloaded-operator"
  "-modernize-use-trailing-return-type")

set_smart_tidy(queue_test
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  COGNITIVE_IGNORE_MACROS
  EXCLUDE
    "${CLANG_TIDY_PROJECT_WORK_OPTS}"
  HAS_EXCEPTIONS)

set_smart_tidy(queue_performance_test
  BUILD_PATH "${CMAKE_BINARY_DIR}"
  EXCLUDE
    "-llvm-prefer-static-over-anonymous-namespace"
    "${CLANG_TIDY_PROJECT_WORK_OPTS}"
  EXTRA_INCLUDES
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)
