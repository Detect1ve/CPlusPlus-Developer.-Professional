cmake_minimum_required(VERSION 4.0.3)

set(PATCH_VERSION
    "1"
    CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(ip_filter VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-Wall -Wextra -pedantic)

add_library(ip_filter_lib STATIC lib/ip_filter.cpp)
target_include_directories(ip_filter_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(ip_filter src/main.cpp)
target_link_libraries(ip_filter PRIVATE ip_filter_lib -static)

set_target_properties(ip_filter ip_filter_lib PROPERTIES CXX_STANDARD 26)

if(PROJECT_IS_TOP_LEVEL)
  add_compile_options(-fdiagnostics-color=always)
  include(cmake/googletest.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

enable_testing()
add_executable(ip_filter_test test/ip_filter_test.cpp)
set_target_properties(ip_filter_test PROPERTIES CXX_STANDARD 26)
target_link_libraries(ip_filter_test PRIVATE ip_filter_lib GTest::gtest_main crypto)
add_test(
  NAME HW_2.ip_filter_test
  COMMAND
    ${CMAKE_COMMAND} -Dexe:FILEPATH=$<TARGET_FILE:ip_filter_test>
    -Dtest_data_file=${CMAKE_CURRENT_SOURCE_DIR}/test/ip_filter.tsv -P
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/stdin_pipe.cmake)

install(TARGETS ip_filter RUNTIME DESTINATION bin
                                  COMPONENT "${PROJECT_NAME}")
if(PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_2_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
