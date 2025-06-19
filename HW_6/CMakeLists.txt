cmake_minimum_required(VERSION 4.0.3)

set(PATCH_VERSION
    "1"
    CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(matrix VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-Wall -Wextra -pedantic)

add_executable(matrix src/main.cpp)

target_include_directories(matrix PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(matrix
  PROPERTIES CXX_STANDARD 23)

add_executable(matrix_test test/matrix_test.cpp)

target_link_libraries(matrix_test PRIVATE GTest::gtest_main)
set_target_properties(matrix_test PROPERTIES CXX_STANDARD 23)
target_include_directories(matrix_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

enable_testing()
add_test(HW_6.matrix_test matrix_test)

install(TARGETS matrix RUNTIME DESTINATION bin
                               COMPONENT "${PROJECT_NAME}")

if(PROJECT_IS_TOP_LEVEL)
  add_compile_options(-fdiagnostics-color=always)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_6_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
