cmake_minimum_required(VERSION 4.0.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(bayan VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  if (MSVC)
    set(SUPPORTED_CXX_STANDARDS 23)
  else()
    set(SUPPORTED_CXX_STANDARDS 26 23)
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
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(../boost.cmake)
  include(../clang-tidy.cmake)
  include(../googletest.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_library(bayan_lib STATIC lib/bayan.cpp)
target_include_directories(bayan_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(bayan_lib
  PRIVATE Boost::crc Boost::program_options Boost::uuid Boost::algorithm
  PUBLIC Boost::filesystem)
target_compile_options(bayan_lib PRIVATE -Wall -Wextra -pedantic)

add_executable(bayan src/main.cpp)
target_link_libraries(bayan PRIVATE bayan_lib)
if (NOT MSVC)
  target_link_libraries(bayan PRIVATE -static)
endif()
target_compile_options(bayan PRIVATE -Wall -Wextra -pedantic)

find_package(absl)
if (NOT absl_FOUND)
  FetchContent_Declare(
    absl
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    GIT_TAG 20250512.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL)

  FetchContent_MakeAvailable(absl)
endif()

add_executable(bayan_test test/bayan_test.cpp)
target_link_libraries(bayan_test PRIVATE bayan_lib GTest::gtest_main absl::strings)
target_compile_options(bayan_test PRIVATE -Wall -Wextra -pedantic)

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set(CLANG_TIDY_HW_8_COMMON_OPTS
    "-altera-struct-pack-align,\
     -fuchsia-default-arguments-calls,\
     -llvmlibc-callee-namespace,\
     -llvmlibc-restrict-system-libc-headers,\
     -misc-include-cleaner,\
     -modernize-use-trailing-return-type;--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(bayan_lib PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_COMMON_OPTS},\
      -altera-id-dependent-backward-branch,\
      -altera-unroll-loops,\
      -llvmlibc-implementation-in-namespace,\
      ${CLANG_TIDY_HW_8_COMMON_OPTS}")

  set_target_properties(bayan PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_COMMON_OPTS},\
      -llvmlibc-implementation-in-namespace,\
      ${CLANG_TIDY_HW_8_COMMON_OPTS}")

  set_target_properties(bayan_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_COMMON_OPTS},\
      ${CLANG_TIDY_HW_8_COMMON_OPTS};--config=\
      {\
        CheckOptions:\
        [\
          {\
            key: readability-function-cognitive-complexity.IgnoreMacros, value: 'true'\
          }\
        ]\
      }")
endif()

enable_testing()
add_test(HW_8.bayan_test bayan_test)

install(TARGETS bayan RUNTIME DESTINATION bin
                      COMPONENT "${PROJECT_NAME}")
if (PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_8_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
