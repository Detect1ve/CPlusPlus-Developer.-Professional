cmake_minimum_required(VERSION 3.13.3)

project(C++-Developer-Professional LANGUAGES CXX)

include(googletest.cmake)

add_subdirectory(HW_1)

enable_testing()

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
set(CPACK_DEB_COMPONENT_INSTALL ON)

set(CPACK_DEBIAN_HELLOWORLD_FILE_NAME ${HW_1_DEB_NAME})
set(CPACK_DEBIAN_HELLOWORLD_PACKAGE_NAME ${HW_1_DEB_NAME})

get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
include(CPack)
