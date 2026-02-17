cmake_minimum_required(VERSION 3.25..4.2.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(bulk VERSION ${PROJECT_VERSION} LANGUAGES CXX)

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
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/lto_enable.cmake)
  include(../cmake/warning_flags.cmake)
  include(../common/attribute_wrapper.cmake)
  include(../common/capture.cmake)
  include(../common/custom_print.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(HW_7_COMPILE_WARNING_FLAGS -Wc++20-compat-pedantic)
endif()

add_library(taskmanager STATIC lib/taskmanager.cpp)
target_include_directories(taskmanager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(taskmanager
  PRIVATE custom_print
  PUBLIC attribute_wrapper)
target_compile_options(taskmanager PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_7_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(taskmanager
    PRIVATE -Wno-strict-overflow
    PUBLIC -Wno-effc++)
endif()

add_executable(bulk src/main.cpp)
target_link_libraries(bulk PRIVATE custom_print taskmanager)
target_compile_options(bulk PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_7_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_link_libraries(bulk PRIVATE -static)
  target_compile_options(bulk PRIVATE
    -Wno-unsafe-buffer-usage-in-container -Wno-unsafe-buffer-usage)
endif()

add_executable(taskmanager_test test/taskmanager_test.cpp)
target_link_libraries(taskmanager_test PRIVATE
  capture custom_print GTest::gtest_main taskmanager)
target_compile_options(taskmanager_test PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_7_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(taskmanager_test PRIVATE
    -Wno-unsafe-buffer-usage -Wno-global-constructors)
endif()

enable_lto(bulk taskmanager taskmanager_test)

set(CLANG_TIDY_HW_7_OPTS
  "-fuchsia-default-arguments-calls"
  "-modernize-use-trailing-return-type")

set_smart_tidy(taskmanager
  EXCLUDE
    "-fuchsia-overloaded-operator"
    "-llvm-prefer-static-over-anonymous-namespace"
    "${CLANG_TIDY_HW_7_OPTS}"
  EXTRA_INCLUDES
    "${ATTRIBUTE_WRAPPER_INCLUDE_DIR}"
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_tidy(bulk
  EXCLUDE
    "${CLANG_TIDY_HW_7_OPTS}"
  EXTRA_INCLUDES
    "${ATTRIBUTE_WRAPPER_INCLUDE_DIR}"
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_tidy(taskmanager_test
  EXCLUDE
    "-llvm-prefer-static-over-anonymous-namespace"
    "${CLANG_TIDY_HW_7_OPTS}"
  EXTRA_INCLUDES
    "${ATTRIBUTE_WRAPPER_INCLUDE_DIR}"
    "${CAPTURE_INCLUDE_DIR}"
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

enable_testing()
add_test(NAME HW_7.taskmanager_test COMMAND $<TARGET_FILE:taskmanager_test>)

install(TARGETS bulk RUNTIME DESTINATION bin
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
