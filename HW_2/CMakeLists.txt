cmake_minimum_required(VERSION 3.31.6)

set(PATCH_VERSION
    "1"
    CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(ip_filter VERSION ${PROJECT_VERSION})

set(HW_2_DEB_NAME
    "${PROJECT_NAME}-${PROJECT_VERSION}.deb"
    PARENT_SCOPE)

add_compile_options(-Wall -Wextra -pedantic -fdiagnostics-color=always)

add_library(ip_filter STATIC lib/ip_filter.cpp)

target_include_directories(ip_filter PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(ip_filter_cli src/main.cpp)

set_target_properties(ip_filter_cli ip_filter PROPERTIES CXX_STANDARD 23)

target_link_libraries(ip_filter_cli PRIVATE ip_filter -static)

find_package(GTest 1.16.0)
if(NOT GTest_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    GTest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL ON)
  FetchContent_MakeAvailable(GTest)
endif()

enable_testing()
add_executable(ip_filter_test test/ip_filter_test.cpp)
set_target_properties(ip_filter_test PROPERTIES CXX_STANDARD 23)

target_link_libraries(ip_filter_test PRIVATE ip_filter GTest::gtest_main crypto)

add_test(
  NAME HW_2.ip_filter_test
  COMMAND
    ${CMAKE_COMMAND} -Dexe:FILEPATH=$<TARGET_FILE:ip_filter_test>
    -Dtest_data_file=${CMAKE_CURRENT_SOURCE_DIR}/test/ip_filter.tsv -P
    ${CMAKE_CURRENT_SOURCE_DIR}/stdin_pipe.cmake)

install(TARGETS ip_filter_cli RUNTIME DESTINATION bin
                                      COMPONENT "${PROJECT_NAME}")
