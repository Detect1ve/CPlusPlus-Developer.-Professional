cmake_minimum_required(VERSION 3.25..4.1.0)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(fashio_mnist VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  if (NOT MSVC)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      add_compile_options(-fcolor-diagnostics)
    else()
      add_compile_options(-fdiagnostics-color=always)
    endif()
  endif()

  set(MINIMUM_CXX_STANDARD 14)
  include(../cmake/cxx_standard.cmake)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if (PROJECT_IS_TOP_LEVEL)
    include(../cmake/clang-tidy.cmake)
    include(../cmake/fetch_googletest.cmake)
    include(../cmake/gcc_warnings.cmake)
    list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

include(../common/eigen3.cmake)

add_library(mlp_lib STATIC lib/mlp.cpp)
target_link_libraries(mlp_lib PRIVATE eigen3_eigen)
target_include_directories(mlp_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
if (MSVC)
  target_compile_options(mlp_lib PRIVATE /W4)
else()
  target_compile_options(mlp_lib PRIVATE ${GCC_FLAGS} -Wno-effc++)
endif()

add_executable(fashio_mnist src/main.cpp)
target_link_libraries(fashio_mnist mlp_lib)
if (MSVC)
  target_compile_options(fashio_mnist PRIVATE /W4)
else()
  target_link_libraries(fashio_mnist -static)
  target_compile_options(fashio_mnist PRIVATE ${GCC_FLAGS})
endif()

add_executable(fashio_mnist_test test/mlp_test.cpp)
target_link_libraries(fashio_mnist_test mlp_lib GTest::gtest_main)
if (MSVC)
  target_compile_options(fashio_mnist_test PRIVATE /W4)
else()
  target_compile_options(fashio_mnist_test PRIVATE ${GCC_FLAGS})
endif()

target_compile_definitions(fashio_mnist_test PRIVATE
  PROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set(CLANG_TIDY_HW_13_OPTS
    "-fuchsia-default-arguments-calls,\
     -llvmlibc-callee-namespace,\
     -llvmlibc-restrict-system-libc-headers,\
     -misc-include-cleaner,\
     -modernize-use-trailing-return-type;--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")

  set_target_properties(mlp_lib PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-id-dependent-backward-branch,\
      -altera-unroll-loops,\
      -cppcoreguidelines-avoid-magic-numbers,\
      -llvmlibc-implementation-in-namespace,\
      -readability-magic-numbers,\
      ${CLANG_TIDY_HW_13_OPTS}")

  set_target_properties(fashio_mnist PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -llvmlibc-implementation-in-namespace,\
      ${CLANG_TIDY_HW_13_OPTS}")

  set_target_properties(fashio_mnist_test PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      ${CLANG_TIDY_HW_13_OPTS}")
endif()

enable_testing()
add_test(HW_13.fashio_mnist_test fashio_mnist_test)

install(TARGETS fashio_mnist
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
