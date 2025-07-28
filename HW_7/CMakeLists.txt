cmake_minimum_required(VERSION 4.0.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(bulk VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  if (MSVC)
    set(SUPPORTED_CXX_STANDARDS 23 20)
  else()
    set(SUPPORTED_CXX_STANDARDS 26 23 20)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      add_compile_options(-fcolor-diagnostics)
    else()
      add_compile_options(-fdiagnostics-color=always)
    endif()
  endif()

  include(../cxx_standard.cmake)
  set(CMAKE_CXX_STANDARD ${CXX_STANDARD})
  set(CMAKE_CXX_STANDARD_REQUIRED True)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(../clang-tidy.cmake)
  include(../googletest.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_library(taskmanager STATIC lib/taskmanager.cpp)
target_include_directories(taskmanager PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(bulk src/main.cpp)
target_link_libraries(bulk PRIVATE taskmanager)
if (NOT MSVC)
  target_link_libraries(bulk PRIVATE -static)
endif()

add_executable(taskmanager_test test/taskmanager_test.cpp)
target_link_libraries(taskmanager_test PRIVATE taskmanager GTest::gtest_main)

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set(CLANG_TIDY_HW_7_COMMON_OPTS
    "-fuchsia-default-arguments-calls,\
     -llvmlibc-callee-namespace,\
     -llvmlibc-implementation-in-namespace,\
     -llvmlibc-inline-function-decl,\
     -llvmlibc-restrict-system-libc-headers,\
     -misc-include-cleaner,\
     -modernize-use-trailing-return-type;--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(taskmanager PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_COMMON_OPTS},\
      -altera-unroll-loops,\
      -fuchsia-overloaded-operator,\
      ${CLANG_TIDY_HW_7_COMMON_OPTS}")

  set_target_properties(bulk PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_COMMON_OPTS},\
      ${CLANG_TIDY_HW_7_COMMON_OPTS}")

  set_target_properties(taskmanager_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_COMMON_OPTS},\
      -altera-unroll-loops,\
      -cppcoreguidelines-pro-bounds-pointer-arithmetic,\
      -fuchsia-statically-constructed-objects,\
      ${CLANG_TIDY_HW_7_COMMON_OPTS}")
endif()

enable_testing()
add_test(HW_7.taskmanager_test taskmanager_test)

install(TARGETS bulk RUNTIME DESTINATION bin
                     COMPONENT "${PROJECT_NAME}")
if (PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_7_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
