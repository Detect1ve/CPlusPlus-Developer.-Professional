cmake_minimum_required(VERSION 3.25..4.1.0)
project(ProducerConsumerQueue LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  if (NOT MSVC)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      add_compile_options(-fcolor-diagnostics)
    else()
      add_compile_options(-fdiagnostics-color=always)
    endif()
  endif()

  set(MINIMUM_CXX_STANDARD 17)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(../cmake/clang-tidy.cmake)
  include(../cmake/googletest.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_executable(queue_test test/queue_test.cpp)
target_include_directories(queue_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(queue_test PRIVATE GTest::gtest_main)

enable_testing()
add_test(Project_work.queue_test queue_test)

add_executable(queue_performance_test test/queue_performance_test.cpp)
target_include_directories(queue_performance_test PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set(CLANG_TIDY_PROJECT_WORK_OPTS
    "-altera-id-dependent-backward-branch,\
     -altera-struct-pack-align,\
     -altera-unroll-loops,\
     -fuchsia-default-arguments-calls,\
     -fuchsia-default-arguments-declarations,\
     -fuchsia-overloaded-operator,\
     -llvmlibc-callee-namespace,\
     -llvmlibc-implementation-in-namespace,\
     -llvmlibc-inline-function-decl,\
     -llvmlibc-restrict-system-libc-headers,\
     -misc-include-cleaner,\
     -modernize-use-trailing-return-type;--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(queue_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      ${CLANG_TIDY_PROJECT_WORK_OPTS};--config=\
      {\
        CheckOptions:\
        [\
          {\
            key: readability-function-cognitive-complexity.IgnoreMacros, value: 'true'\
          }\
        ]\
      }")

  set_target_properties(queue_performance_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      ${CLANG_TIDY_PROJECT_WORK_OPTS}")
endif()
