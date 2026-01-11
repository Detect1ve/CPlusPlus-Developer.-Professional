cmake_minimum_required(VERSION 3.25..4.2.1)

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
  include(../common/capture.cmake)
  include(../common/socket_wrapper.cmake)
  include(../cmake/clang-tidy.cmake)
  include(../cmake/fetch_boost.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/gcc_warnings.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(HW_10_COMPILE_WARNING_FLAGS -Wc++17-compat-pedantic)
endif()

add_library(server_async STATIC lib/async.cpp)
target_include_directories(server_async PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(server_async PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_10_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(server_async PRIVATE -Wno-effc++)
endif()

add_library(server STATIC lib/server.cpp)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(server PRIVATE Boost::asio server_async)
target_compile_options(server PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_10_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(server PRIVATE -Wno-effc++ -Wno-null-dereference)
endif()

add_executable(bulk_server src/main.cpp)
target_link_libraries(bulk_server PRIVATE server)
target_compile_options(bulk_server PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_10_COMPILE_WARNING_FLAGS})
if (MSVC)
  target_compile_definitions(server PRIVATE _WIN32_WINNT=0x0A00)
  target_compile_definitions(bulk_server PRIVATE _WIN32_WINNT=0x0A00)
else()
  target_compile_options(bulk_server
    PRIVATE -Wno-unsafe-buffer-usage-in-container -Wno-unsafe-buffer-usage)
  target_link_libraries(bulk_server PRIVATE -static)
endif()

add_executable(bulk_server_test test/bulk_server_test.cpp)
target_link_libraries(bulk_server_test PRIVATE capture GTest::gtest_main server
  socket_wrapper)
target_compile_options(bulk_server_test PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_10_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(bulk_server_test
      PRIVATE -Wno-alloc-zero -Wno-strict-overflow)
  else()
    target_compile_options(bulk_server_test
      PRIVATE -Wno-unsafe-buffer-usage -Wno-global-constructors)
  endif()
endif()

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  if (MSVC)
    set(CLANG_TIDY_MSVC_OPTS ";--extra-arg=/EHsc")
  endif()

  set(CLANG_TIDY_HW_10_OPTS
    "-llvm-header-guard,\
     -llvm-namespace-comment,\
     -llvmlibc-callee-namespace,\
     -llvmlibc-implementation-in-namespace,\
     -llvmlibc-restrict-system-libc-headers,\
     -misc-include-cleaner,\
     -modernize-use-trailing-return-type,\
     -portability-avoid-pragma-once")

  set_target_properties(server_async PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-id-dependent-backward-branch,\
      -altera-struct-pack-align,\
      -altera-unroll-loops,\
      -fuchsia-default-arguments-calls,\
      -llvm-prefer-static-over-anonymous-namespace,\
      ${CLANG_TIDY_HW_10_OPTS};--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(server PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-unroll-loops,\
      -fuchsia-default-arguments-calls,\
      -llvm-prefer-static-over-anonymous-namespace,\
      -llvmlibc-inline-function-decl,\
      -misc-no-recursion,\
      ${CLANG_TIDY_HW_10_OPTS};--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(bulk_server PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      ${CLANG_TIDY_HW_10_OPTS}${CLANG_TIDY_MSVC_OPTS};--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(bulk_server_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-id-dependent-backward-branch,\
      -altera-unroll-loops,\
      -fuchsia-default-arguments-calls,\
      -llvm-prefer-static-over-anonymous-namespace,\
      -llvmlibc-inline-function-decl,\
      ${CLANG_TIDY_HW_10_OPTS}${CLANG_TIDY_MSVC_OPTS};--config=\
      {\
        CheckOptions:\
        [\
          {\
            key: readability-function-cognitive-complexity.IgnoreMacros, value: 'true'\
          }\
        ]\
      };--header-filter=(${CMAKE_CURRENT_SOURCE_DIR}/include/.*|${CAPTURE_INCLUDE_DIR}/.*|${SOCKET_WRAPPER_INCLUDE_DIR}/.*)")
endif()

enable_testing()
add_test(NAME HW_10.bulk_server_test COMMAND $<TARGET_FILE:bulk_server_test>)

install(TARGETS bulk_server
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
