cmake_minimum_required(VERSION 4.0.3)

project(HW_5 LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  if (MSVC)
    set(CMAKE_CXX_STANDARD 23)
  else()
    set(CMAKE_CXX_STANDARD 26)
    add_compile_options(-fdiagnostics-color=always)
  endif()

  set(CMAKE_CXX_STANDARD_REQUIRED True)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -pedantic)
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
