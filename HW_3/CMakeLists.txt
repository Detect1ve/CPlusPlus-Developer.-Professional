cmake_minimum_required(VERSION 4.0.3)

project(HW_3 LANGUAGES CXX)

add_compile_options(-Wall -Wextra -pedantic)

add_executable(main src/main.cpp)
target_include_directories(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(main PRIVATE -static)
set_target_properties(main PROPERTIES
  CXX_STANDARD 26
  CXX_STANDARD_REQUIRED ON)

if(PROJECT_IS_TOP_LEVEL)
  add_compile_options(-fdiagnostics-color=always)
  include(cmake/googletest.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_executable(my_allocator_test test/my_allocator_test.cpp)
target_include_directories(my_allocator_test PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(my_allocator_test PRIVATE GTest::gtest_main)

add_executable(my_list_test test/my_list_test.cpp)
target_include_directories(my_list_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(my_list_test PRIVATE GTest::gtest_main)

set_target_properties(my_allocator_test my_list_test PROPERTIES
  CXX_STANDARD 26
  CXX_STANDARD_REQUIRED ON)

enable_testing()
add_test(HW_3.my_allocator_test my_allocator_test)
add_test(HW_3.my_list_test my_list_test)
