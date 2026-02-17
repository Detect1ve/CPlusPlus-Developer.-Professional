cmake_minimum_required(VERSION 3.25..4.2.3)

project(HW_5 LANGUAGES CXX)

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
  include(../cmake/lto_enable.cmake)
  include(../cmake/warning_flags.cmake)
  include(../common/attribute_wrapper.cmake)
  include(../common/custom_print.cmake)
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
target_link_libraries(editor PRIVATE attribute_wrapper custom_print)
target_compile_options(editor PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_5_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_link_libraries(editor PRIVATE -static)
  target_compile_options(editor PRIVATE -Wno-effc++)
endif()

enable_lto(editor)

set_smart_tidy(editor
  EXCLUDE
    "-altera-struct-pack-align"
    "-fuchsia-default-arguments-calls"
    "-modernize-use-trailing-return-type"
  EXTRA_INCLUDES
    "${ATTRIBUTE_WRAPPER_INCLUDE_DIR}"
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)
