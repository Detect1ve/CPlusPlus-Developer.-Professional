cmake_minimum_required(VERSION 3.31.6)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(helloworld VERSION ${PROJECT_VERSION} LANGUAGES CXX)

configure_file(version.h.in version.h)

add_compile_options(-fdiagnostics-color=always)

add_executable(helloworld_cli main.cpp)
add_library(helloworld STATIC lib.cpp)

set_target_properties(helloworld_cli helloworld PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(helloworld
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(helloworld_cli PRIVATE
    helloworld -static
)

find_package(Boost 1.87.0 QUIET CONFIG COMPONENTS unit_test_framework)
if(NOT Boost_FOUND)
  include(FetchContent)
  set(BOOST_INCLUDE_LIBRARIES test)
  FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.tar.xz
    URL_MD5 d55d43218e81ca3d0fc14436b7665bf1
    DOWNLOAD_NO_PROGRESS ON
    EXCLUDE_FROM_ALL)
  FetchContent_MakeAvailable(Boost)
endif()

if(PROJECT_IS_TOP_LEVEL)
  include(googletest.cmake)
endif()

add_executable(test_version test_version.cpp)
add_executable(g_test_version g_test_version.cpp)
set_target_properties(test_version g_test_version PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(test_version
    PRIVATE Boost::unit_test_framework
    helloworld
)

target_link_libraries(g_test_version
    PRIVATE GTest::gtest_main
    helloworld
)

if (MSVC)
    target_compile_options(helloworld_cli PRIVATE
        /W4
    )
    target_compile_options(helloworld PRIVATE
        /W4
    )
    target_compile_options(test_version PRIVATE
        /W4
    )
else ()
    target_compile_options(helloworld_cli PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(helloworld PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(test_version PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()

enable_testing()
add_test(HW_1.test_version test_version)
add_test(HW_1.g_test_version g_test_version)

install(TARGETS helloworld_cli RUNTIME DESTINATION bin
                                       COMPONENT "${PROJECT_NAME}")

if(PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_1_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
