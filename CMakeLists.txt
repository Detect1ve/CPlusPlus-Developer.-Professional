cmake_minimum_required(VERSION 4.0.3)

project(C++-Developer-Professional LANGUAGES CXX)

add_compile_options(-fdiagnostics-color=always)

list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

include(googletest.cmake)

add_subdirectory(HW_1)
add_subdirectory(HW_2)
add_subdirectory(HW_3)
add_subdirectory(HW_4)
add_subdirectory(HW_5)

enable_testing()

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
set(CPACK_DEB_COMPONENT_INSTALL ON)

set(CPACK_DEBIAN_HELLOWORLD_FILE_NAME ${HW_1_DEB_NAME})
set(CPACK_DEBIAN_HELLOWORLD_PACKAGE_NAME ${HW_1_DEB_NAME})

set(CPACK_DEBIAN_IP_FILTER_FILE_NAME ${HW_2_DEB_NAME})
set(CPACK_DEBIAN_IP_FILTER_PACKAGE_NAME ${HW_2_DEB_NAME})

set(CPACK_DEBIAN_PRINT_IP_FILE_NAME ${HW_4_DEB_NAME})
set(CPACK_DEBIAN_PRINT_IP_PACKAGE_NAME ${HW_4_DEB_NAME})

# For some unknown reason, FetchContent_Declare for GTest adds extra components.
# It seems to be a bug in CMake.
get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
list(REMOVE_ITEM CPACK_COMPONENTS_ALL "gmock" "gtest")
include(CPack)
