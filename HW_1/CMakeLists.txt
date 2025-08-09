cmake_minimum_required(VERSION 3.25..4.1.0)

if (NOT DEFINED PATCH_VERSION)
    set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
endif()
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(helloworld VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
    if (NOT MSVC)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            add_compile_options(-fcolor-diagnostics)
        else ()
            add_compile_options(-fdiagnostics-color=always)
        endif()
    endif()
    set(MINIMUM_CXX_STANDARD 98)
    include(../cmake/cxx_standard.cmake)
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
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
    include(../cmake/boost.cmake)
    include(../cmake/clang-tidy.cmake)
    include(../cmake/googletest.cmake)
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

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
    set_target_properties(helloworld_cli PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
            -hicpp-named-parameter,\
            -llvmlibc-callee-namespace,\
            -llvmlibc-implementation-in-namespace,\
            -llvmlibc-restrict-system-libc-headers,\
            -llvm-header-guard,\
            -modernize-use-trailing-return-type,\
            -performance-avoid-endl,\
            -readability-named-parameter"
    )

    set_target_properties(helloworld PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
            -cppcoreguidelines-macro-to-enum,\
            -cppcoreguidelines-macro-usage,\
            -llvmlibc-implementation-in-namespace,\
            -llvm-header-guard,\
            -modernize-macro-to-enum,\
            -modernize-use-trailing-return-type"
    )

    set_target_properties(test_version PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
            -altera-unroll-loops,\
            -cert-err58-cpp,\
            -cppcoreguidelines-avoid-do-while,\
            -cppcoreguidelines-macro-usage,\
            -cppcoreguidelines-pro-type-vararg,\
            -fuchsia-default-arguments-calls,\
            -fuchsia-statically-constructed-objects,\
            -hicpp-vararg,\
            -llvmlibc-callee-namespace,\
            -llvmlibc-implementation-in-namespace,\
            -misc-include-cleaner,\
            -readability-identifier-length"
    )

    set_target_properties(g_test_version PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
            -cert-err58-cpp,\
            -cppcoreguidelines-owning-memory,\
            -fuchsia-default-arguments-calls,\
            -fuchsia-statically-constructed-objects,\
            -llvmlibc-callee-namespace,\
            -llvmlibc-restrict-system-libc-headers,\
            -modernize-use-trailing-return-type"
    )
endif()

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
                                       COMPONENT "${PROJECT_NAME}"
)

if (PROJECT_IS_TOP_LEVEL)
    set(CPACK_GENERATOR DEB)
    set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
    include(CPack)
else ()
    set(HW_1_DEB_NAME
        "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
        PARENT_SCOPE
    )
endif()

enable_testing()
add_test(HW_1.test_version test_version)
add_test(HW_1.g_test_version g_test_version)
