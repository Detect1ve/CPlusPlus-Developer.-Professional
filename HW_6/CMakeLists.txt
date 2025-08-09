cmake_minimum_required(VERSION 3.25..4.1.0)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(matrix VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  if (NOT MSVC)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      add_compile_options(-fcolor-diagnostics)
    else()
      add_compile_options(-fdiagnostics-color=always)
    endif()
  endif()

  set(MINIMUM_CXX_STANDARD 11)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(../cmake/capture.cmake)
  include(../cmake/clang-tidy.cmake)
  include(../cmake/googletest.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_executable(matrix src/main.cpp)
target_include_directories(matrix PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if (NOT MSVC)
  target_link_libraries(matrix PRIVATE -static)
endif()

add_executable(matrix_test test/matrix_test.cpp)
target_include_directories(matrix_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(matrix_test PRIVATE capture GTest::gtest_main)

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set(CLANG_TIDY_HW_6_OPTS
    "-altera-unroll-loops,\
     -fuchsia-overloaded-operator,\
     -llvmlibc-callee-namespace,\
     -llvmlibc-implementation-in-namespace,\
     -llvmlibc-inline-function-decl,\
     -llvmlibc-restrict-system-libc-headers,\
     -modernize-use-trailing-return-type;--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(matrix PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      ${CLANG_TIDY_HW_6_OPTS}")

  set_target_properties(matrix_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -cppcoreguidelines-avoid-magic-numbers,\
      -fuchsia-default-arguments-calls,\
      -misc-include-cleaner,\
      -performance-avoid-endl,\
      -readability-identifier-length,\
      -readability-magic-numbers,\
      ${CLANG_TIDY_HW_6_OPTS};--config=\
      {\
        CheckOptions:\
        [\
          {\
            key: readability-function-cognitive-complexity.IgnoreMacros, value: 'true'\
          }\
        ]\
      };--header-filter=${CAPTURE_INCLUDE_DIR}/.*")
endif()

enable_testing()
add_test(HW_6.matrix_test matrix_test)

install(TARGETS matrix RUNTIME DESTINATION bin
                               COMPONENT "${PROJECT_NAME}")
if (PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_6_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
