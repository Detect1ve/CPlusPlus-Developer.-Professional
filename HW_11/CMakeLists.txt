cmake_minimum_required(VERSION 4.0.3)

if(NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(join_server VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-Wall -Wextra -pedantic)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(PROJECT_IS_TOP_LEVEL)
  add_compile_options(-fdiagnostics-color=always)
  include(cmake/boost.cmake)
endif()

include_directories(include)

set(SOURCES
    src/main.cpp
    src/server.cpp
    src/database.cpp
)
add_executable(join_server ${SOURCES})
target_link_libraries(join_server PRIVATE Boost::asio -static)
install(TARGETS join_server
        RUNTIME DESTINATION bin
        COMPONENT "${PROJECT_NAME}")
if(PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_11_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
