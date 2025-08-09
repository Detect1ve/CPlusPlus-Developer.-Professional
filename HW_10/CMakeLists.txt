cmake_minimum_required(VERSION 3.25..4.1.0)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(bulk_server VERSION ${PROJECT_VERSION} LANGUAGES CXX)

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
  include(../cmake/boost.cmake)
  include(../cmake/capture.cmake)
  include(../cmake/clang-tidy.cmake)
  include(../cmake/googletest.cmake)
  include(../cmake/socket_wrapper.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_library(server_async STATIC lib/async.cpp)
target_include_directories(server_async PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (MSVC)
  target_compile_options(server_async PRIVATE /W4)
else()
  target_compile_options(server_async PRIVATE -Wall -Wextra -pedantic)
endif()

add_library(server STATIC lib/server.cpp)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(server PRIVATE Boost::asio server_async)

if (MSVC)
  target_compile_options(server PRIVATE /W4)
else()
  target_compile_options(server PRIVATE -Wall -Wextra -pedantic)
endif()

add_executable(bulk_server src/main.cpp)
target_link_libraries(bulk_server PRIVATE server)
if (MSVC)
target_compile_options(bulk_server PRIVATE /W4)
  target_compile_definitions(server PRIVATE _WIN32_WINNT=0x0A00)
  target_compile_definitions(bulk_server PRIVATE _WIN32_WINNT=0x0A00)
else()
  target_compile_options(bulk_server PRIVATE -Wall -Wextra -pedantic)
  target_link_libraries(bulk_server PRIVATE -static)
endif()

add_executable(bulk_server_test test/bulk_server_test.cpp)
target_link_libraries(bulk_server_test PRIVATE capture GTest::gtest_main server
  socket_wrapper)

if (MSVC)
  target_compile_options(bulk_server_test PRIVATE /W4)
else()
  target_compile_options(bulk_server_test PRIVATE -Wall -Wextra -pedantic)
endif()

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set(CLANG_TIDY_HW_10_OPTS
    "-llvm-header-guard,\
     -llvm-namespace-comment,\
     -llvmlibc-callee-namespace,\
     -llvmlibc-implementation-in-namespace,\
     -llvmlibc-restrict-system-libc-headers,\
     -misc-include-cleaner,\
     -modernize-use-trailing-return-type;--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(server_async PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-id-dependent-backward-branch,\
      -altera-struct-pack-align,\
      -altera-unroll-loops,\
      -fuchsia-default-arguments-calls,\
      ${CLANG_TIDY_HW_10_OPTS}")

  set_target_properties(server PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-unroll-loops,\
      -fuchsia-default-arguments-calls,\
      -llvmlibc-inline-function-decl,\
      -misc-no-recursion,\
      ${CLANG_TIDY_HW_10_OPTS}")

  set_target_properties(bulk_server PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      ${CLANG_TIDY_HW_10_OPTS}")

  set_target_properties(bulk_server_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-id-dependent-backward-branch,\
      -altera-unroll-loops,\
      -fuchsia-default-arguments-calls,\
      -llvmlibc-inline-function-decl,\
      ${CLANG_TIDY_HW_10_OPTS};--config=\
      {\
        CheckOptions:\
        [\
          {\
            key: readability-function-cognitive-complexity.IgnoreMacros, value: 'true'\
          }\
        ]\
      };--header-filter=${CAPTURE_INCLUDE_DIR}/.*;--header-filter=${SOCKET_WRAPPER_INCLUDE_DIR}/.*")
endif()

enable_testing()
add_test(HW_10.bulk_server_test bulk_server_test)

install(TARGETS bulk_server
        RUNTIME DESTINATION bin
        COMPONENT "${PROJECT_NAME}")
if (PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_10_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
