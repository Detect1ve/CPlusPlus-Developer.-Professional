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

include(common/absl.cmake)
include(common/boost_algorithm.cmake)
include(common/boost_uuid.cmake)
include(common/capture.cmake)

include(cmake/clang-tidy.cmake)
include(cmake/fetch_googletest.cmake)
include(cmake/gcc_warnings.cmake)

set(SUBDIRS
  HW_1
  HW_2
  HW_3
  HW_4
  HW_5
  HW_6
  HW_7
  HW_8)

foreach(SUBDIR ${SUBDIRS})
  check_subdirectory_compatibility(${SUBDIR})
  if (IS_COMPATIBLE)
    add_subdirectory(${SUBDIR})
  endif()
endforeach()

enable_testing()

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
set(CPACK_DEB_COMPONENT_INSTALL ON)

get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
include(CPack)
