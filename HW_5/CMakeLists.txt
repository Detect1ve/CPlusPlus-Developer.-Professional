cmake_minimum_required(VERSION 3.25..4.3.1)

project(HW_5 LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  set(CMAKE_COLOR_DIAGNOSTICS ON)

  set(MINIMUM_CXX_STANDARD 20)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

  include(../cmake/clang-tidy.cmake)
  include(../cmake/cppcheck.cmake)
  include(../cmake/warning_flags.cmake)
  include(../common/attribute_wrapper.cmake)
  include(../common/custom_print.cmake)
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
set_warning_flags(editor
  EXTRA_CLANG
    -Wc++20-compat-pedantic
  EXTRA_GCC
    -Wno-effc++)
if (NOT MSVC)
  target_link_libraries(editor PRIVATE -static)
endif()

set_smart_tidy(editor
  EXCLUDE
    "-altera-struct-pack-align"
    "-fuchsia-default-arguments-calls"
  EXTRA_TARGETS
    attribute_wrapper
    custom_print
  HAS_EXCEPTIONS)

set_smart_cppcheck(editor
  EXCLUDE
    "unmatchedSuppression"
    "unusedFunction"
  LIBRARIES
    "gnu")
