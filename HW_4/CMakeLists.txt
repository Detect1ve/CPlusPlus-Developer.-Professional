cmake_minimum_required(VERSION 4.0.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(print_ip VERSION ${PROJECT_VERSION} LANGUAGES CXX)

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

add_library(ip_address INTERFACE)
target_include_directories(ip_address INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(print_ip src/main.cpp)
target_link_libraries(print_ip PRIVATE ip_address)
if (NOT MSVC)
  target_link_libraries(print_ip PRIVATE -static)
endif()

add_executable(print_ip_test test/sfinae_ip_address_test.cpp)
target_link_libraries(print_ip_test PRIVATE GTest::gtest_main ip_address)

enable_testing()
add_test(HW_4.print_ip_test print_ip_test)

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set(CLANG_TIDY_HW_4_COMMON_OPTS
    "-altera-unroll-loops,\
     -fuchsia-default-arguments-calls,\
     -google-runtime-int,\
     -llvmlibc-callee-namespace,\
     -llvmlibc-restrict-system-libc-headers,\
     -misc-include-cleaner,\
     -modernize-use-trailing-return-type;--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(print_ip PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_COMMON_OPTS},\
      -llvmlibc-implementation-in-namespace,\
      ${CLANG_TIDY_HW_4_COMMON_OPTS}")

  set_target_properties(print_ip_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_COMMON_OPTS},\
      -cppcoreguidelines-avoid-magic-numbers,\
      -readability-magic-numbers,\
      ${CLANG_TIDY_HW_4_COMMON_OPTS}")
endif()

install(TARGETS print_ip RUNTIME DESTINATION bin
                                 COMPONENT "${PROJECT_NAME}")

find_package(Doxygen 1.14.0)
if (NOT Doxygen_FOUND)
  if (WIN32)
    set(DOXYGEN_URL https://github.com/doxygen/doxygen/releases/download/Release_1_14_0/doxygen-1.14.0.windows.x64.bin.zip)
    set(DOXYGEN_MD5 d7bd7814898ea345c2ee004c57ae0177)
  else()
    set(DOXYGEN_URL https://github.com/doxygen/doxygen/releases/download/Release_1_14_0/doxygen-1.14.0.linux.bin.tar.gz)
    set(DOXYGEN_MD5 e761a5097ae20ecccfd02041925f102a)
  endif()
  include(FetchContent)
  FetchContent_Declare(
    Doxygen
    URL ${DOXYGEN_URL}
    URL_MD5 ${DOXYGEN_MD5}
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

set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs CACHE PATH "Path to Doxygen output directory")
set(DOXYGEN_NUM_PROC_THREADS 0)
set(DOXYGEN_WARN_AS_ERROR YES)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)

doxygen_add_docs(
  docs
  ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include README.md
  COMMENT "Generating API documentation with Doxygen"
)

if (PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_4_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
