cmake_minimum_required(VERSION 3.25..4.2.3)

project(HW_3 LANGUAGES CXX)

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
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/fetch_gsl.cmake)
  include(../cmake/warning_flags.cmake)
  include(../common/custom_print.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

if (   CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
   # Some versions of clang suffer from the following problem:
   # https://github.com/llvm/llvm-project/issues/138775
   AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 20.1.8)
  set(HW_3_COMPILE_WARNING_FLAGS -Wc++20-compat-pedantic)
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

set(CLANG_TIDY_HW_3_OPTS
    "-altera-struct-pack-align"
    "-fuchsia-overloaded-operator"
    "-modernize-use-trailing-return-type")

set_smart_tidy(main
  EXCLUDE
    "-llvm-prefer-static-over-anonymous-namespace"
    "${CLANG_TIDY_HW_3_OPTS}"
  EXTRA_INCLUDES
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_tidy(my_allocator_test
  EXCLUDE
    "-cert-err58-cpp"
    "-cppcoreguidelines-owning-memory"
    "-fuchsia-default-arguments-calls"
    "-fuchsia-statically-constructed-objects"
    "${CLANG_TIDY_HW_3_OPTS}"
  HAS_EXCEPTIONS)

set_smart_tidy(my_list_test
  EXCLUDE
    "${CLANG_TIDY_HW_3_OPTS}")

set_smart_cppcheck(main
  EXCLUDE
    "unusedFunction")

set_smart_cppcheck(my_allocator_test
  EXCLUDE
    "unusedFunction")

set_smart_cppcheck(my_list_test
  EXCLUDE
    "unusedFunction"
  LIBRARIES
    "googletest")

enable_testing()
add_test(NAME HW_3.my_allocator_test COMMAND $<TARGET_FILE:my_allocator_test>)
add_test(NAME HW_3.my_list_test COMMAND $<TARGET_FILE:my_list_test>)
