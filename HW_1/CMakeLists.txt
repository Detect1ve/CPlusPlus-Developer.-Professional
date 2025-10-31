cmake_minimum_required(VERSION 3.25..4.2.0)

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
    set(MINIMUM_CXX_STANDARD 11)
    include(../cmake/cxx_standard.cmake)
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

configure_file(version.h.in version.h)

add_executable(helloworld_cli main.cpp)
add_library(helloworld STATIC lib.cpp)

target_include_directories(helloworld
    PRIVATE "${PROJECT_BINARY_DIR}"
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
    include(../cmake/clang-tidy.cmake)
    include(../cmake/fetch_googletest.cmake)
    include(../cmake/gcc_warnings.cmake)
    list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

include(../common/boost_test.cmake)

add_executable(test_version test_version.cpp)

target_link_libraries(test_version
    PRIVATE wrapper_boost_unit_test_framework
    helloworld
)

add_executable(g_test_version g_test_version.cpp)

target_link_libraries(g_test_version
    PRIVATE GTest::gtest_main
    helloworld
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 15)
        set(HW_1_COMPILE_WARNING_FLAGS -Wleading-whitespace=none)
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(HW_1_COMPILE_WARNING_FLAGS -Wc++11-compat-pedantic)
endif()

target_compile_options(helloworld_cli PRIVATE
    ${COMPILE_WARNING_FLAGS} ${HW_1_COMPILE_WARNING_FLAGS}
)
target_compile_options(helloworld PRIVATE
    ${COMPILE_WARNING_FLAGS} ${HW_1_COMPILE_WARNING_FLAGS}
)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(helloworld PRIVATE
        -Wno-suggest-attribute=const 
    )
endif()
target_compile_options(test_version PRIVATE
    ${COMPILE_WARNING_FLAGS} ${HW_1_COMPILE_WARNING_FLAGS}
)
if (NOT MSVC)
    target_compile_options(test_version PRIVATE
        -Wno-used-but-marked-unused -Wno-global-constructors
    )
endif()
target_compile_options(g_test_version PRIVATE
    ${COMPILE_WARNING_FLAGS} ${HW_1_COMPILE_WARNING_FLAGS}
)
if (NOT MSVC)
    target_compile_options(g_test_version PRIVATE
        -Wno-global-constructors
    )
endif()

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

install(TARGETS helloworld_cli RUNTIME DESTINATION bin
                                       COMPONENT "${PROJECT_NAME}"
)

if (PROJECT_IS_TOP_LEVEL)
    set(CPACK_GENERATOR DEB)
    set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
    include(CPack)
else ()
    string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPPER)

    set(CPACK_DEBIAN_${PROJECT_NAME_UPPER}_FILE_NAME
        "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
        PARENT_SCOPE
    )
    set(CPACK_DEBIAN_${PROJECT_NAME_UPPER}_PACKAGE_NAME
        "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
        PARENT_SCOPE
    )
endif()

enable_testing()
add_test(NAME HW_1.test_version COMMAND $<TARGET_FILE:test_version>)
add_test(NAME HW_1.g_test_version COMMAND $<TARGET_FILE:g_test_version>)
