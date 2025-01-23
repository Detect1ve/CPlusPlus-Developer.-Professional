cmake_minimum_required(VERSION 3.25..4.2.0)

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

if (PROJECT_IS_TOP_LEVEL)
  include(../common/boost_algorithm.cmake)
  include(../common/boost_uuid.cmake)
  include(../common/capture.cmake)
  include(../cmake/clang-tidy.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/gcc_warnings.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(HW_2_COMPILE_WARNING_FLAGS -Wc++20-compat-pedantic)
endif()

add_library(ip_filter_lib STATIC lib/ip_filter.cpp)
target_include_directories(ip_filter_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(ip_filter_lib PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_2_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(ip_filter_lib
    PRIVATE -Wno-strict-overflow -Wno-unsafe-buffer-usage)
endif()

add_executable(ip_filter src/main.cpp)
target_link_libraries(ip_filter PRIVATE ip_filter_lib)
if (NOT MSVC)
  target_link_libraries(ip_filter PRIVATE -static)
endif()
target_compile_options(ip_filter PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_2_COMPILE_WARNING_FLAGS})

add_executable(ip_filter_test test/ip_filter_test.cpp)
target_link_libraries(ip_filter_test
  PRIVATE capture GTest::gtest_main ip_filter_lib wrapper_boost_algorithm
    wrapper_boost_uuid)
target_compile_options(ip_filter_test PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_2_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(ip_filter_test PRIVATE -Wno-global-constructors)
endif()

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set(CLANG_TIDY_HW_2_OPTS
    "-altera-id-dependent-backward-branch,\
     -altera-unroll-loops,\
     -llvmlibc-callee-namespace,\
     -llvmlibc-implementation-in-namespace,\
     -llvmlibc-inline-function-decl,\
     -llvmlibc-restrict-system-libc-headers,\
     -misc-include-cleaner,\
     -modernize-use-trailing-return-type")

  set_target_properties(ip_filter_lib PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -fuchsia-default-arguments-calls,\
      -llvm-prefer-static-over-anonymous-namespace,\
      ${CLANG_TIDY_HW_2_OPTS};--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(ip_filter PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -google-readability-todo,\
      ${CLANG_TIDY_HW_2_OPTS};--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(ip_filter_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -fuchsia-default-arguments-calls,\
      -llvm-prefer-static-over-anonymous-namespace,\
      ${CLANG_TIDY_HW_2_OPTS};--header-filter=(${CMAKE_CURRENT_SOURCE_DIR}/include/.*|${CAPTURE_INCLUDE_DIR}/.*)")
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
  string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPPER)

  set(CPACK_DEBIAN_${PROJECT_NAME_UPPER}_FILE_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)

  set(CPACK_DEBIAN_${PROJECT_NAME_UPPER}_PACKAGE_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
