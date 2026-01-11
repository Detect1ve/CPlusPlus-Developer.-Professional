cmake_minimum_required(VERSION 3.25..4.2.1)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PATCH_COMPILE_COMMANDS FALSE)

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(print_ip VERSION ${PROJECT_VERSION} LANGUAGES CXX)

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
  include(../cmake/clang-tidy.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/gcc_warnings.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(HW_4_COMPILE_WARNING_FLAGS -Wc++17-compat-pedantic)
endif()

add_library(ip_address INTERFACE)
target_include_directories(ip_address INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS_EQUAL 9.4)
    target_compile_options(ip_address INTERFACE -fconcepts)
    set(PATCH_COMPILE_COMMANDS TRUE)
  endif()
endif()

add_executable(print_ip src/main.cpp)
target_link_libraries(print_ip PRIVATE ip_address)
if (NOT MSVC)
  target_link_libraries(print_ip PRIVATE -static)
endif()
target_compile_options(print_ip PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_4_COMPILE_WARNING_FLAGS})

add_executable(print_ip_test test/sfinae_ip_address_test.cpp)
target_link_libraries(print_ip_test PRIVATE capture GTest::gtest_main ip_address)
target_compile_options(print_ip_test PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_4_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(print_ip_test PRIVATE -Wno-global-constructors)
endif()

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set(CLANG_TIDY_HW_4_OPTS
    "-altera-unroll-loops,\
     -cppcoreguidelines-avoid-magic-numbers,\
     -fuchsia-default-arguments-calls,\
     -google-runtime-int,\
     -llvmlibc-callee-namespace,\
     -llvmlibc-restrict-system-libc-headers,\
     -misc-include-cleaner,\
     -modernize-use-trailing-return-type,\
     -readability-magic-numbers;-p;${CMAKE_BINARY_DIR}")

  set_target_properties(print_ip PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -llvmlibc-implementation-in-namespace,\
      ${CLANG_TIDY_HW_4_OPTS};--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(print_ip_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -llvmlibc-inline-function-decl,\
      ${CLANG_TIDY_HW_4_OPTS};--header-filter=(${CMAKE_CURRENT_SOURCE_DIR}/include/.*|${CAPTURE_INCLUDE_DIR}/.*)")

  if (PATCH_COMPILE_COMMANDS)
    add_custom_target(patch_compile_commands ALL
      COMMAND sed -i 's/-fconcepts//g' ${CMAKE_BINARY_DIR}/compile_commands.json
      DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
      COMMENT "Patching compile_commands.json for clang-tidy")

    add_dependencies(print_ip patch_compile_commands)
    add_dependencies(print_ip_test patch_compile_commands)
  endif()
endif()

enable_testing()
add_test(NAME HW_4.print_ip_test COMMAND $<TARGET_FILE:print_ip_test>)

install(TARGETS print_ip RUNTIME DESTINATION bin
                                 COMPONENT "${PROJECT_NAME}")

find_package(Doxygen 1.16.1)
if (NOT Doxygen_FOUND)
  if (WIN32)
    set(DOXYGEN_URL https://github.com/doxygen/doxygen/releases/download/Release_1_16_1/doxygen-1.16.1.windows.x64.bin.zip)
    set(DOXYGEN_SHA256 ddfa59a4ae9549651330471c2e387ec7a9891080543faed541c859e5ce448653)
  else()
    set(DOXYGEN_URL https://github.com/doxygen/doxygen/releases/download/Release_1_16_1/doxygen-1.16.1.linux.bin.tar.gz)
    set(DOXYGEN_SHA256 a56f885d37e3aae08a99f638d17bbb381224c03a878d9e2dda4f9fa4baf1d8bd)
  endif()
  include(FetchContent)
  FetchContent_Declare(
    Doxygen
    URL ${DOXYGEN_URL}
    URL_HASH SHA256=${DOXYGEN_SHA256}
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    EXCLUDE_FROM_ALL)
  FetchContent_MakeAvailable(Doxygen)

  if (WIN32)
    set(DOXYGEN_EXECUTABLE "${doxygen_SOURCE_DIR}/doxygen.exe")
  else()
    set(DOXYGEN_EXECUTABLE "${doxygen_SOURCE_DIR}/bin/doxygen")
  endif()
  include(FindDoxygen)
endif()

set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs CACHE
  PATH "Path to Doxygen output directory")
set(DOXYGEN_NUM_PROC_THREADS 0)
set(DOXYGEN_WARN_AS_ERROR YES)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)

if (Doxygen_FOUND)
  doxygen_add_docs(
    docs
    ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include README.md
    COMMENT "Generating API documentation with Doxygen")
else()
  message(WARNING "Doxygen not found. Building documentation is disabled.")
endif()

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
