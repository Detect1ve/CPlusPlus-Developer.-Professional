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
  include(../common/absl.cmake)
  include(../common/capture.cmake)
  include(../cmake/clang-tidy.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/gcc_warnings.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

if (MSVC)
  add_compile_options(/W4)
endif()

add_library(async SHARED lib/async.cpp include/async.h)
target_include_directories(async PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
if (MSVC)
  set_target_properties(async PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
else()
  target_compile_options(async PRIVATE ${GCC_FLAGS} -Wno-effc++ -Wno-strict-overflow)
endif()

add_library(async_static STATIC lib/async.cpp)
target_include_directories(async_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
if (NOT MSVC)
  target_compile_options(async_static PRIVATE ${GCC_FLAGS} -Wno-effc++ -Wno-strict-overflow)
endif()

add_executable(async_cli src/main.cpp)
target_link_libraries(async_cli PRIVATE async_static)
if (NOT MSVC)
  target_link_libraries(async_cli PRIVATE -static)
endif()
target_compile_options(async_cli PRIVATE ${GCC_FLAGS})

add_executable(async_test test/async_test.cpp)
target_link_libraries(async_test
  PRIVATE async capture GTest::gtest_main wrapper_absl_strings)
target_compile_options(async_test PRIVATE ${GCC_FLAGS})

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set(CLANG_TIDY_HW_9_OPTS
    "-llvm-header-guard,\
     -llvm-namespace-comment,\
     -llvmlibc-callee-namespace,\
     -llvmlibc-implementation-in-namespace,\
     -llvmlibc-restrict-system-libc-headers,\
     -misc-include-cleaner,\
     -modernize-use-trailing-return-type,\
     -portability-avoid-pragma-once")

  set_target_properties(async PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-id-dependent-backward-branch,\
      -altera-struct-pack-align,\
      -altera-unroll-loops,\
      -fuchsia-default-arguments-calls,\
      -llvm-prefer-static-over-anonymous-namespace,\
      ${CLANG_TIDY_HW_9_OPTS};--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(async_static PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-id-dependent-backward-branch,\
      -altera-struct-pack-align,\
      -altera-unroll-loops,\
      -fuchsia-default-arguments-calls,\
      -llvm-prefer-static-over-anonymous-namespace,\
      ${CLANG_TIDY_HW_9_OPTS};--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(async_cli PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -cppcoreguidelines-avoid-magic-numbers,\
      -hicpp-named-parameter,\
      -llvm-qualified-auto,\
      -misc-const-correctness,\
      -readability-identifier-length,\
      -readability-magic-numbers,\
      -readability-named-parameter,\
      -readability-qualified-auto,\
      ${CLANG_TIDY_HW_9_OPTS};--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

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
      };--header-filter=(${CMAKE_CURRENT_SOURCE_DIR}/include/.*|${CAPTURE_INCLUDE_DIR}/.*)")
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
  string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPPER)

  set(CPACK_DEBIAN_${PROJECT_NAME_UPPER}_FILE_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)

  set(CPACK_DEBIAN_${PROJECT_NAME_UPPER}_PACKAGE_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
