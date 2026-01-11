cmake_minimum_required(VERSION 3.25..4.2.1)

project(HW_5 LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  set(CMAKE_BUILD_TYPE MinSizeRel)
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
  include(../cmake/gcc_warnings.cmake)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(HW_5_COMPILE_WARNING_FLAGS -Wc++20-compat-pedantic)
endif()

add_executable(editor
  src/controller/document_controller.cpp
  src/controller/primitive_controller.cpp
  src/model/primitives/line.cpp
  src/model/document.cpp
  src/model/primitive.cpp
  src/view/document_view.cpp
  src/view/primitive_view.cpp
  src/main.cpp)
target_include_directories(editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(editor PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_5_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_link_libraries(editor PRIVATE -static)
  target_compile_options(editor PRIVATE -Wno-effc++)
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
