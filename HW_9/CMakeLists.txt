cmake_minimum_required(VERSION 3.25..4.1.2)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(async VERSION ${PROJECT_VERSION} LANGUAGES CXX)

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
  include(../cmake/absl.cmake)
  include(../cmake/capture.cmake)
  include(../cmake/clang-tidy.cmake)
  include(../cmake/googletest.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

add_library(async SHARED lib/async.cpp include/async.h)
target_include_directories(async PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
if (MSVC)
  set_target_properties(async PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_library(async_static STATIC lib/async.cpp)
target_include_directories(async_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(async_cli src/main.cpp)
target_link_libraries(async_cli PRIVATE async_static)
if (NOT MSVC)
  target_link_libraries(async_cli PRIVATE -static)
endif()

add_executable(async_test test/async_test.cpp)
target_link_libraries(async_test PRIVATE absl::strings async capture GTest::gtest_main)

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set(CLANG_TIDY_HW_9_OPTS
    "-llvm-header-guard,\
     -llvm-namespace-comment,\
     -llvmlibc-callee-namespace,\
     -llvmlibc-implementation-in-namespace,\
     -llvmlibc-restrict-system-libc-headers,\
     -misc-include-cleaner,\
     -modernize-use-trailing-return-type;--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(async PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-id-dependent-backward-branch,\
      -altera-struct-pack-align,\
      -altera-unroll-loops,\
      -fuchsia-default-arguments-calls,\
      -llvm-prefer-static-over-anonymous-namespace,\
      -portability-avoid-pragma-once,\
      ${CLANG_TIDY_HW_9_OPTS}")

  set_target_properties(async_static PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-id-dependent-backward-branch,\
      -altera-struct-pack-align,\
      -altera-unroll-loops,\
      -fuchsia-default-arguments-calls,\
      -llvm-prefer-static-over-anonymous-namespace,\
      -portability-avoid-pragma-once,\
      ${CLANG_TIDY_HW_9_OPTS}")

  set_target_properties(async_cli PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -cppcoreguidelines-avoid-magic-numbers,\
      -hicpp-named-parameter,\
      -llvm-qualified-auto,\
      -misc-const-correctness,\
      -portability-avoid-pragma-once,\
      -readability-identifier-length,\
      -readability-magic-numbers,\
      -readability-named-parameter,\
      -readability-qualified-auto,\
      ${CLANG_TIDY_HW_9_OPTS}")

  set_target_properties(async_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-unroll-loops,\
      -fuchsia-default-arguments-calls,\
      -llvm-prefer-static-over-anonymous-namespace,\
      -llvmlibc-inline-function-decl,\
      ${CLANG_TIDY_HW_9_OPTS};--config=\
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
add_test(HW_9.async_test async_test)

install(TARGETS async
        LIBRARY DESTINATION lib
        COMPONENT "${PROJECT_NAME}")
install(FILES include/async.h
        DESTINATION include/async
        COMPONENT "${PROJECT_NAME}")
install(TARGETS async_cli
        RUNTIME DESTINATION bin
        COMPONENT "${PROJECT_NAME}")
if (PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_9_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
