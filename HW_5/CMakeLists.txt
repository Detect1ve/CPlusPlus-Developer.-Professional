# cmake_minimum_required(VERSION 4.0.3)
cmake_minimum_required(VERSION 3.31.4)

set(PATCH_VERSION
    "1"
    CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(GraphicEditor VERSION ${PROJECT_VERSION} LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB_RECURSE SOURCES
    "src/*.cpp"
)

file(GLOB_RECURSE HEADERS
    "include/*.hpp"
)

add_executable(editor ${SOURCES} ${HEADERS})

set_target_properties(editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# target_compile_features(editor PRIVATE cxx_std_26)
target_compile_features(editor PRIVATE cxx_std_23)
