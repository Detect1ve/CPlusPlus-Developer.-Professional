cmake_minimum_required(VERSION 3.25..4.2.0)

project(HW_3 LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  if (NOT MSVC)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      add_compile_options(-fcolor-diagnostics)
    else()
      add_compile_options(-fdiagnostics-color=always)
    endif()
  endif()

  set(MINIMUM_CXX_STANDARD 14)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(../cmake/clang-tidy.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/fetch_gsl.cmake)
  include(../cmake/gcc_warnings.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(HW_3_COMPILE_WARNING_FLAGS -Wc++14-compat-pedantic)
endif()

add_library(list_and_allocator INTERFACE)
target_include_directories(list_and_allocator
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(list_and_allocator INTERFACE Microsoft.GSL::GSL)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE list_and_allocator)
if (NOT MSVC)
  target_link_libraries(main PRIVATE -static)
endif()
target_compile_options(main PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_3_COMPILE_WARNING_FLAGS})

add_executable(my_allocator_test test/my_allocator_test.cpp)
target_link_libraries(my_allocator_test PRIVATE GTest::gtest_main list_and_allocator)
target_compile_options(my_allocator_test PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_3_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(my_allocator_test PRIVATE -Wno-global-constructors)
endif()

add_executable(my_list_test test/my_list_test.cpp)
target_link_libraries(my_list_test PRIVATE GTest::gtest_main list_and_allocator)
target_compile_options(my_list_test PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_3_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(my_list_test PRIVATE -Wno-global-constructors)
endif()

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set(CLANG_TIDY_HW_3_OPTS
    "-altera-id-dependent-backward-branch,\
     -altera-struct-pack-align,\
     -altera-unroll-loops,\
     -fuchsia-overloaded-operator,\
     -llvmlibc-inline-function-decl,\
     -llvmlibc-restrict-system-libc-headers,\
     -modernize-use-trailing-return-type;--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(main PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -llvm-prefer-static-over-anonymous-namespace,\
      -llvmlibc-callee-namespace,\
      -llvmlibc-implementation-in-namespace,\
      -misc-include-cleaner,\
      ${CLANG_TIDY_HW_3_OPTS}")

  set_target_properties(my_allocator_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -cert-err58-cpp,\
      -cppcoreguidelines-owning-memory,\
      -fuchsia-default-arguments-calls,\
      -fuchsia-statically-constructed-objects,\
      -llvmlibc-implementation-in-namespace,\
      -misc-include-cleaner,\
      ${CLANG_TIDY_HW_3_OPTS}")

  set_target_properties(my_list_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -llvmlibc-callee-namespace,\
      ${CLANG_TIDY_HW_3_OPTS}")
endif()

enable_testing()
add_test(NAME HW_3.my_allocator_test COMMAND $<TARGET_FILE:my_allocator_test>)
add_test(NAME HW_3.my_list_test COMMAND $<TARGET_FILE:my_list_test>)
