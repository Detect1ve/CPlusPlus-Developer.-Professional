cmake_minimum_required(VERSION 3.25..4.2.0)

project(C++-Developer-Professional LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

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

include(common/boost_algorithm.cmake)
include(common/boost_uuid.cmake)
include(common/capture.cmake)

include(cmake/clang-tidy.cmake)
include(cmake/fetch_googletest.cmake)
include(cmake/gcc_warnings.cmake)

check_subdirectory_compatibility(HW_1)
if (IS_COMPATIBLE)
  add_subdirectory(HW_1)
endif()

check_subdirectory_compatibility(HW_2)
if (IS_COMPATIBLE)
  add_subdirectory(HW_2)
endif()

check_subdirectory_compatibility(HW_3)
if (IS_COMPATIBLE)
  add_subdirectory(HW_3)
endif()

check_subdirectory_compatibility(HW_4)
if (IS_COMPATIBLE)
  add_subdirectory(HW_4)
endif()

enable_testing()

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
set(CPACK_DEB_COMPONENT_INSTALL ON)

get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
include(CPack)
