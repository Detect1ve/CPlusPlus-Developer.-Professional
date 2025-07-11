cmake_minimum_required(VERSION 4.0.3)

project(HW_3 LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  if (MSVC)
    set(CMAKE_CXX_STANDARD 23)
  else ()
    set(CMAKE_CXX_STANDARD 26)
    add_compile_options(-fdiagnostics-color=always)
  endif()

  set(CMAKE_CXX_STANDARD_REQUIRED True)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(../googletest.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_executable(main src/main.cpp)
target_include_directories(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
if (NOT MSVC)
  target_link_libraries(main PRIVATE -static)
endif()

add_executable(my_allocator_test test/my_allocator_test.cpp)
target_include_directories(my_allocator_test PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(my_allocator_test PRIVATE GTest::gtest_main)

add_executable(my_list_test test/my_list_test.cpp)
target_include_directories(my_list_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(my_list_test PRIVATE GTest::gtest_main)

enable_testing()
add_test(HW_3.my_allocator_test my_allocator_test)
add_test(HW_3.my_list_test my_list_test)
