cmake_minimum_required(VERSION 4.0.3)

if (NOT DEFINED PATCH_VERSION)
    set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
endif()
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(helloworld VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
    if (MSVC)
        set(CMAKE_CXX_STANDARD 23)
    else ()
        set(CMAKE_CXX_STANDARD 26)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            add_compile_options(-fcolor-diagnostics)
        else ()
            add_compile_options(-fdiagnostics-color=always)
        endif()
    endif()
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

configure_file(version.h.in version.h)

add_executable(helloworld_cli main.cpp)
add_library(helloworld STATIC lib.cpp)

target_include_directories(helloworld
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(helloworld_cli PRIVATE
    helloworld
)

if (NOT MSVC)
    target_link_libraries(helloworld_cli PRIVATE
        -static
    )
endif()

if (PROJECT_IS_TOP_LEVEL)
    include(../boost.cmake)
    include(../googletest.cmake)
    list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_executable(test_version test_version.cpp)

target_link_libraries(test_version
    PRIVATE Boost::unit_test_framework
    helloworld
)

add_executable(g_test_version g_test_version.cpp)

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
if (PROJECT_IS_TOP_LEVEL)
    set(CPACK_GENERATOR DEB)
    set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
    include(CPack)
else ()
    set(HW_1_DEB_NAME
        "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
        PARENT_SCOPE)
endif()

enable_testing()
add_test(HW_1.test_version test_version)
add_test(HW_1.g_test_version g_test_version)
