cmake_minimum_required(VERSION 3.25..4.1.0)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(ip_filter VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  if (NOT MSVC)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      add_compile_options(-fcolor-diagnostics)
    else()
      add_compile_options(-fdiagnostics-color=always)
    endif()
  endif()

  set(MINIMUM_CXX_STANDARD 23)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(../cmake/boost.cmake)
  include(../cmake/capture.cmake)
  include(../cmake/clang-tidy.cmake)
  include(../cmake/googletest.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_library(ip_filter_lib STATIC lib/ip_filter.cpp)
target_include_directories(ip_filter_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(ip_filter src/main.cpp)
target_link_libraries(ip_filter PRIVATE ip_filter_lib)
if (NOT MSVC)
  target_link_libraries(ip_filter PRIVATE -static)
endif()

add_executable(ip_filter_test test/ip_filter_test.cpp)
target_link_libraries(ip_filter_test PRIVATE Boost::algorithm Boost::uuid capture
  GTest::gtest_main ip_filter_lib)

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set(CLANG_TIDY_HW_2_OPTS
    "-altera-id-dependent-backward-branch,\
     -altera-unroll-loops,\
     -llvmlibc-callee-namespace,\
     -llvmlibc-implementation-in-namespace,\
     -llvmlibc-inline-function-decl,\
     -llvmlibc-restrict-system-libc-headers,\
     -misc-include-cleaner,\
     -modernize-use-trailing-return-type;--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(ip_filter_lib PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -fuchsia-default-arguments-calls,\
      ${CLANG_TIDY_HW_2_OPTS}")

  set_target_properties(ip_filter PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -google-readability-todo,\
      ${CLANG_TIDY_HW_2_OPTS}")

  set_target_properties(ip_filter_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -fuchsia-default-arguments-calls,\
      ${CLANG_TIDY_HW_2_OPTS};--header-filter=${CAPTURE_INCLUDE_DIR}/.*")
endif()

enable_testing()
add_test(
  NAME HW_2.ip_filter_test
  COMMAND
    ${CMAKE_COMMAND} -Dexe:FILEPATH=$<TARGET_FILE:ip_filter_test>
    -Dtest_data_file=${CMAKE_CURRENT_SOURCE_DIR}/test/ip_filter.tsv -P
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/stdin_pipe.cmake)

install(TARGETS ip_filter RUNTIME DESTINATION bin
                                  COMPONENT "${PROJECT_NAME}")
if (PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_2_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
