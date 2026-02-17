cmake_minimum_required(VERSION 3.25..4.2.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(join_server VERSION ${PROJECT_VERSION} LANGUAGES CXX)

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
  include(../cmake/fetch_boost.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/warning_flags.cmake)
  include(../common/custom_print.cmake)
  include(../common/socket_wrapper.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(HW_11_COMPILE_WARNING_FLAGS -Wc++17-compat-pedantic)
endif()

add_library(server_and_database STATIC lib/server.cpp lib/database.cpp)
target_include_directories(server_and_database PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(server_and_database PRIVATE Boost::asio custom_print)
target_compile_options(server_and_database PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_11_COMPILE_WARNING_FLAGS})
if (MSVC)
  target_compile_definitions(server_and_database PRIVATE _WIN32_WINNT=0x0A00)
else()
  target_compile_options(server_and_database PRIVATE
    -Wno-effc++ -Wno-null-dereference -Wno-strict-overflow -Wno-unsafe-buffer-usage)
endif()

add_executable(join_server src/main.cpp)
target_link_libraries(join_server PRIVATE custom_print server_and_database)
target_compile_options(join_server PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_11_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(join_server PRIVATE
    -Wno-unsafe-buffer-usage-in-container -Wno-unsafe-buffer-usage)
  target_link_libraries(join_server PRIVATE -static)
endif()

add_executable(join_server_test test/join_server_test.cpp)
target_link_libraries(join_server_test PRIVATE GTest::gtest_main server_and_database
  socket_wrapper)
target_compile_options(join_server_test PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_11_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(join_server_test PRIVATE
    -Wno-effc++ -Wno-strict-overflow -Wno-global-constructors)
endif()

set(CLANG_TIDY_HW_11_OPTS
    "-modernize-use-trailing-return-type")

set_smart_tidy(server_and_database
  EXCLUDE
    "-altera-struct-pack-align"
    "-fuchsia-default-arguments-calls"
    "-llvm-prefer-static-over-anonymous-namespace"
    "-misc-no-recursion"
    "${CLANG_TIDY_HW_11_OPTS}"
  EXTRA_INCLUDES
    "${CUSTOM_PRINT_INCLUDE_DIR}")

set_smart_tidy(join_server
  EXCLUDE
    "${CLANG_TIDY_HW_11_OPTS}"
  EXTRA_INCLUDES
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_tidy(join_server_test
  EXCLUDE
    "-fuchsia-default-arguments-calls"
    "${CLANG_TIDY_HW_11_OPTS}"
  EXTRA_INCLUDES
    "${SOCKET_WRAPPER_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

enable_testing()
add_test(NAME HW_11.join_server_test COMMAND $<TARGET_FILE:join_server_test>)

install(TARGETS join_server
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
