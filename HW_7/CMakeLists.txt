cmake_minimum_required(VERSION 4.0.3)

if(NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(bulk VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
  add_compile_options(-fdiagnostics-color=always)
  include(cmake/googletest.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_compile_options(-Wall -Wextra -pedantic)

add_library(taskmanager STATIC lib/taskmanager.cpp)
target_include_directories(taskmanager PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(bulk src/main.cpp)
target_link_libraries(bulk PRIVATE taskmanager -static)

set_target_properties(bulk taskmanager PROPERTIES
  CXX_STANDARD 26
  CXX_STANDARD_REQUIRED ON)

install(TARGETS bulk RUNTIME DESTINATION bin
                     COMPONENT "${PROJECT_NAME}")

add_executable(taskmanager_test test/taskmanager_test.cpp)
target_link_libraries(taskmanager_test PRIVATE taskmanager GTest::gtest_main)
set_target_properties(taskmanager_test PROPERTIES
  CXX_STANDARD 26
  CXX_STANDARD_REQUIRED ON)

enable_testing()
add_test(HW_7.taskmanager_test taskmanager_test)

if(PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_7_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
