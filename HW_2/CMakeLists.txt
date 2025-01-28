cmake_minimum_required(VERSION 3.13.3)

add_compile_options(-Wall -Wextra -pedantic)

add_library(ip_filter lib/ip_filter.cpp)

target_include_directories(ip_filter
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(main src/main.cpp)

set_property(TARGET main ip_filter PROPERTY CXX_STANDARD 20)

target_link_libraries(main PRIVATE
    ip_filter
)

find_package(GTest)
if (GTest_FOUND)
    enable_testing()
    add_executable(ip_filter_test test/ip_filter_test.cpp)

    target_link_libraries(
        ip_filter_test
        ip_filter
        ${GTEST_BOTH_LIBRARIES}
        crypto
    )

    add_test(NAME HW_2.ip_filter_test
        COMMAND ${CMAKE_COMMAND} -Dexe:FILEPATH=$<TARGET_FILE:ip_filter_test> -P ${CMAKE_CURRENT_SOURCE_DIR}/stdin_pipe.cmake
    )
else()
    message(WARNING "Skip GoogleTest testing")
endif()
