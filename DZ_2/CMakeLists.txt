cmake_minimum_required(VERSION 3.13.3)

add_executable(ip_filter ip_filter.cpp)

set_property(TARGET ip_filter PROPERTY CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -pedantic)
