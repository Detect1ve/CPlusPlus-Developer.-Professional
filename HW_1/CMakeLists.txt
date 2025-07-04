cmake_minimum_required(VERSION 4.0.3)

if(NOT DEFINED PATCH_VERSION)
    set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
endif()
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(helloworld VERSION ${PROJECT_VERSION} LANGUAGES CXX)

configure_file(version.h.in version.h)

add_executable(helloworld_cli main.cpp)
add_library(helloworld STATIC lib.cpp)

set_target_properties(helloworld_cli helloworld PROPERTIES
    CXX_STANDARD 26
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(helloworld
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(helloworld_cli PRIVATE
    helloworld -static
)

if(PROJECT_IS_TOP_LEVEL)
    add_compile_options(-fdiagnostics-color=always)
    include(cmake/boost.cmake)
    include(cmake/googletest.cmake)
    list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_executable(test_version test_version.cpp)

set_target_properties(test_version PROPERTIES
    CXX_STANDARD 26
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(test_version
    PRIVATE Boost::unit_test_framework
    helloworld
)

add_executable(g_test_version g_test_version.cpp)

set_target_properties(test_version g_test_version PROPERTIES
    CXX_STANDARD 26
    CXX_STANDARD_REQUIRED ON
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
    target_compile_options(g_test_version PRIVATE
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
    target_compile_options(g_test_version PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()

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

enable_testing()
add_test(HW_1.test_version test_version)
add_test(HW_1.g_test_version g_test_version)
