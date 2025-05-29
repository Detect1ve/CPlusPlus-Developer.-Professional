cmake_minimum_required(VERSION 4.0.3)

set(PATCH_VERSION
    "1"
    CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(ProducerConsumerQueue VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(PROJECT_IS_TOP_LEVEL)
    add_compile_options(-fdiagnostics-color=always)
    include(cmake/googletest.cmake)
    list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_executable(queue_test test/queue_test.cpp)
target_include_directories(queue_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(queue_test PRIVATE GTest::gtest_main)

enable_testing()
add_test(Project_work.queue_test queue_test)

add_executable(queue_performance_test test/queue_performance_test.cpp)
target_include_directories(queue_performance_test PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
