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
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(../cmake/clang-tidy.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/gcc_warnings.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_library(data_queue INTERFACE)
target_include_directories(data_queue INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if (NOT MSVC)
  target_compile_options(data_queue INTERFACE -Wno-effc++ -Wno-strict-overflow)
endif()

add_executable(queue_test test/queue_test.cpp)
target_link_libraries(queue_test PRIVATE data_queue GTest::gtest_main)
target_compile_options(queue_test PRIVATE ${GCC_FLAGS})

enable_testing()
add_test(Project_work.queue_test queue_test)

add_executable(queue_performance_test test/queue_performance_test.cpp)
target_link_libraries(queue_performance_test PRIVATE data_queue)
target_compile_options(queue_performance_test PRIVATE ${GCC_FLAGS})

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
      -llvm-prefer-static-over-anonymous-namespace,\
      ${CLANG_TIDY_PROJECT_WORK_OPTS}")
endif()
