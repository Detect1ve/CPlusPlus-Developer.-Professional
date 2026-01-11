cmake_minimum_required(VERSION 3.25..4.2.0)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(bayan VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  if (NOT MSVC)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      add_compile_options(-fcolor-diagnostics)
    else()
      add_compile_options(-fdiagnostics-color=always)
    endif()
  endif()

  set(MINIMUM_CXX_STANDARD 23)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(../common/capture.cmake)
  include(../cmake/clang-tidy.cmake)
  include(../cmake/fetch_absl.cmake)
  include(../cmake/fetch_boost.cmake)
  include(../cmake/fetch_googletest.cmake)
  include(../cmake/gcc_warnings.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(HW_8_COMPILE_WARNING_FLAGS -Wc++17-compat-pedantic)
endif()

add_library(bayan_lib STATIC lib/bayan.cpp)
target_include_directories(bayan_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(bayan_lib PRIVATE
  Boost::algorithm Boost::crc Boost::filesystem Boost::program_options Boost::uuid)
target_compile_options(bayan_lib PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_8_COMPILE_WARNING_FLAGS})
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(bayan_lib
    PRIVATE -Wno-alloc-zero -Wno-effc++ -Wno-strict-overflow)
endif()

add_executable(bayan src/main.cpp)
target_link_libraries(bayan PRIVATE bayan_lib)
if (NOT MSVC)
  target_link_libraries(bayan PRIVATE -static)
endif()
target_compile_options(bayan PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_8_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(bayan PRIVATE -Wno-unsafe-buffer-usage-in-container)
endif()

add_executable(bayan_test test/bayan_test.cpp)
target_link_libraries(bayan_test PRIVATE
  absl::strings bayan_lib Boost::filesystem capture GTest::gtest_main)
target_compile_options(bayan_test PRIVATE
  ${COMPILE_WARNING_FLAGS} ${HW_8_COMPILE_WARNING_FLAGS})
if (NOT MSVC)
  target_compile_options(bayan_test PRIVATE -Wno-global-constructors)
endif()

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set(CLANG_TIDY_HW_8_OPTS
    "-altera-struct-pack-align,\
     -fuchsia-default-arguments-calls,\
     -llvmlibc-callee-namespace,\
     -llvmlibc-inline-function-decl,\
     -llvmlibc-restrict-system-libc-headers,\
     -misc-include-cleaner,\
     -modernize-use-trailing-return-type")

  set_target_properties(bayan_lib PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-id-dependent-backward-branch,\
      -altera-unroll-loops,\
      -llvmlibc-implementation-in-namespace,\
      ${CLANG_TIDY_HW_8_OPTS};--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(bayan PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -llvmlibc-implementation-in-namespace,\
      ${CLANG_TIDY_HW_8_OPTS};--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(bayan_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      ${CLANG_TIDY_HW_8_OPTS};--config=\
      {\
        CheckOptions:\
        [\
          {\
            key: readability-function-cognitive-complexity.IgnoreMacros, value: 'true'\
          }\
        ]\
      };--header-filter=(${CMAKE_CURRENT_SOURCE_DIR}/include/.*|${CAPTURE_INCLUDE_DIR}/.*)")
endif()

enable_testing()
add_test(NAME HW_8.bayan_test COMMAND $<TARGET_FILE:bayan_test>)

install(TARGETS bayan RUNTIME DESTINATION bin
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
