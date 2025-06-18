cmake_minimum_required(VERSION 3.25..4.1.2)

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

add_subdirectory_if_compatible(HW_1)
add_subdirectory_if_compatible(HW_2)
add_subdirectory_if_compatible(HW_3)
add_subdirectory_if_compatible(HW_4)
add_subdirectory_if_compatible(HW_5)

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

include(CPack)
