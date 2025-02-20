cmake_minimum_required(VERSION 4.0.3)

project(C++-Developer-Professional LANGUAGES CXX)

if (MSVC)
  set(CMAKE_CXX_STANDARD 23)
else ()
  set(CMAKE_CXX_STANDARD 26)
  add_compile_options(-fdiagnostics-color=always)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

include(googletest.cmake)

add_subdirectory(HW_1)
add_subdirectory(HW_2)
add_subdirectory(HW_3)

enable_testing()

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
set(CPACK_DEB_COMPONENT_INSTALL ON)

set(CPACK_DEBIAN_HELLOWORLD_FILE_NAME ${HW_1_DEB_NAME})
set(CPACK_DEBIAN_HELLOWORLD_PACKAGE_NAME ${HW_1_DEB_NAME})

set(CPACK_DEBIAN_IP_FILTER_FILE_NAME ${HW_2_DEB_NAME})
set(CPACK_DEBIAN_IP_FILTER_PACKAGE_NAME ${HW_2_DEB_NAME})

# For some unknown reason, FetchContent_Declare for GTest adds extra components.
# It seems to be a bug in CMake.
get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
list(REMOVE_ITEM CPACK_COMPONENTS_ALL "gmock" "gtest")
include(CPack)
