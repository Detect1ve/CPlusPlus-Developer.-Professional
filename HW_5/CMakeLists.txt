cmake_minimum_required(VERSION 4.0.3)

project(HW_5 LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  if (MSVC)
    set(SUPPORTED_CXX_STANDARDS 23 20 17 14)
  else()
    set(SUPPORTED_CXX_STANDARDS 26 23 20 17 14)
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
endif()

file(GLOB_RECURSE SOURCES
    "src/*.cpp"
)

add_executable(editor ${SOURCES})
target_include_directories(editor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
if (NOT MSVC)
  target_link_libraries(editor PRIVATE -static)
endif()

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set_target_properties(editor PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-struct-pack-align,\
      -altera-unroll-loops,\
      -fuchsia-default-arguments-calls,\
      -llvmlibc-callee-namespace,\
      -llvmlibc-implementation-in-namespace,\
      -llvmlibc-inline-function-decl,\
      -llvmlibc-restrict-system-libc-headers,\
      -misc-include-cleaner,\
      -modernize-use-trailing-return-type;--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")
endif()
