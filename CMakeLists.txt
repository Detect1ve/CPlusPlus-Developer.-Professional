cmake_minimum_required(VERSION 3.25..4.1.0)

project(C++-Developer-Professional LANGUAGES CXX)

if (NOT MSVC)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
  else()
    add_compile_options(-fdiagnostics-color=always)
  endif()
endif()

include(cmake/cxx_standard.cmake)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

include(cmake/boost.cmake)
include(cmake/capture.cmake)
include(cmake/clang-tidy.cmake)
include(cmake/googletest.cmake)
include(cmake/socket_wrapper.cmake)

add_subdirectory_if_compatible(HW_1)
add_subdirectory_if_compatible(HW_2)
add_subdirectory_if_compatible(HW_3)
add_subdirectory_if_compatible(HW_4)
add_subdirectory_if_compatible(HW_5)
add_subdirectory_if_compatible(HW_6)
add_subdirectory_if_compatible(HW_7)
add_subdirectory_if_compatible(HW_8)
add_subdirectory_if_compatible(HW_9)
add_subdirectory_if_compatible(HW_10)
add_subdirectory_if_compatible(HW_11)
add_subdirectory_if_compatible(HW_12)
add_subdirectory_if_compatible(HW_13)
add_subdirectory_if_compatible(Project_work)

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

set(CPACK_DEBIAN_MATRIX_FILE_NAME ${HW_6_DEB_NAME})
set(CPACK_DEBIAN_MATRIX_PACKAGE_NAME ${HW_6_DEB_NAME})

set(CPACK_DEBIAN_BULK_FILE_NAME ${HW_7_DEB_NAME})
set(CPACK_DEBIAN_BULK_PACKAGE_NAME ${HW_7_DEB_NAME})

set(CPACK_DEBIAN_BAYAN_FILE_NAME ${HW_8_DEB_NAME})
set(CPACK_DEBIAN_BAYAN_PACKAGE_NAME ${HW_8_DEB_NAME})

set(CPACK_DEBIAN_ASYNC_FILE_NAME ${HW_9_DEB_NAME})
set(CPACK_DEBIAN_ASYNC_PACKAGE_NAME ${HW_9_DEB_NAME})

set(CPACK_DEBIAN_BULK_SERVER_FILE_NAME ${HW_10_DEB_NAME})
set(CPACK_DEBIAN_BULK_SERVER_PACKAGE_NAME ${HW_10_DEB_NAME})

set(CPACK_DEBIAN_JOIN_SERVER_FILE_NAME ${HW_11_DEB_NAME})
set(CPACK_DEBIAN_JOIN_SERVER_PACKAGE_NAME ${HW_11_DEB_NAME})

set(CPACK_DEBIAN_MAPREDUCE_FILE_NAME ${HW_12_DEB_NAME})
set(CPACK_DEBIAN_MAPREDUCE_PACKAGE_NAME ${HW_12_DEB_NAME})

set(CPACK_DEBIAN_FASHIO_MNIST_FILE_NAME ${HW_13_DEB_NAME})
set(CPACK_DEBIAN_FASHIO_MNIST_PACKAGE_NAME ${HW_13_DEB_NAME})

# For some known reason, FetchContent_Declare for Eigen3 adds extra components.
# Report the issue and remove this workaround when it is fixed.
get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
list(REMOVE_ITEM CPACK_COMPONENTS_ALL "Devel" "Unspecified")
include(CPack)
