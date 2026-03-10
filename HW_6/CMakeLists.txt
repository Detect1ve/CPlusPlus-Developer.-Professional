cmake_minimum_required(VERSION 3.25..4.2.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(matrix VERSION ${PROJECT_VERSION} LANGUAGES CXX)

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
  include(../cmake/cppcheck.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/warning_flags.cmake)
  include(../common/capture.cmake)
  include(../common/custom_print.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(HW_6_COMPILE_WARNING_FLAGS -Wc++20-compat-pedantic)
endif()

add_executable(matrix src/main.cpp)
target_include_directories(matrix PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(matrix PRIVATE custom_print)
if (NOT MSVC)
  target_link_libraries(matrix PRIVATE -static)
endif()
target_compile_options(matrix PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_6_COMPILE_WARNING_FLAGS})

add_executable(matrix_test test/matrix_test.cpp)
target_include_directories(matrix_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(matrix_test PRIVATE capture GTest::gtest_main)
target_compile_options(matrix_test PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_6_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(matrix_test PRIVATE -Wno-global-constructors)
endif()

set(CLANG_TIDY_HW_6_OPTS
  "-altera-struct-pack-align"
  "-fuchsia-overloaded-operator"
  "-modernize-use-trailing-return-type")

set_smart_tidy(matrix
  EXCLUDE
    "${CLANG_TIDY_HW_6_OPTS}"
  EXTRA_INCLUDES
    "${CUSTOM_PRINT_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_tidy(matrix_test
  COGNITIVE_IGNORE_MACROS
  EXCLUDE
    "-cppcoreguidelines-avoid-magic-numbers"
    "-fuchsia-default-arguments-calls"
    "-performance-avoid-endl"
    "-readability-identifier-length"
    "-readability-magic-numbers"
    "${CLANG_TIDY_HW_6_OPTS}"
  EXTRA_INCLUDES
    "${CAPTURE_INCLUDE_DIR}"
  HAS_EXCEPTIONS)

set_smart_cppcheck(matrix
  EXCLUDE
    "unusedFunction")

set_smart_cppcheck(matrix_test
  EXCLUDE
    "unusedFunction"
  LIBRARIES
    "googletest,posix")

enable_testing()
add_test(NAME HW_6.matrix_test COMMAND $<TARGET_FILE:matrix_test>)

install(TARGETS matrix RUNTIME DESTINATION bin
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
