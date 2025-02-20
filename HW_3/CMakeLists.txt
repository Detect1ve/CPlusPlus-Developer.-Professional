cmake_minimum_required(VERSION 3.31.4)

add_compile_options(-Wall -Wextra -pedantic -fdiagnostics-color=always)

add_executable(main src/main.cpp)

set_target_properties(main
  PROPERTIES CXX_STANDARD 23)
