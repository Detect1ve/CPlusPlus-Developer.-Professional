cmake_minimum_required(VERSION 4.0.3)

project(HW_5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(PROJECT_IS_TOP_LEVEL)
  add_compile_options(-fdiagnostics-color=always)
endif()

file(GLOB_RECURSE SOURCES
    "src/*.cpp"
)

file(GLOB_RECURSE HEADERS
    "include/*.hpp"
)

add_executable(editor ${SOURCES} ${HEADERS})
target_include_directories(editor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(editor PRIVATE -static)
