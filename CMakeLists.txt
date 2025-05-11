cmake_minimum_required(VERSION 4.0.3)

project(C++-Developer-Professional LANGUAGES CXX)

if (MSVC)
  set(CMAKE_CXX_STANDARD 23)
else ()
  set(CMAKE_CXX_STANDARD 26)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
  else()
    add_compile_options(-fdiagnostics-color=always)
  endif()
endif()

set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

include(boost.cmake)
include(googletest.cmake)

add_subdirectory(HW_1)
add_subdirectory(HW_2)
add_subdirectory(HW_3)
add_subdirectory(HW_4)
add_subdirectory(HW_5)
add_subdirectory(HW_6)
add_subdirectory(HW_7)

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

include(CPack)
