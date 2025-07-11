cmake_minimum_required(VERSION 4.0.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(bulk_server VERSION ${PROJECT_VERSION} LANGUAGES CXX)

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

if (PROJECT_IS_TOP_LEVEL)
  include(../boost.cmake)
endif()

add_library(server_async STATIC lib/async.cpp)
target_include_directories(server_async PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(server STATIC lib/server.cpp)
target_include_directories(server PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(server
  PRIVATE server_async
  PUBLIC Boost::asio)

add_executable(bulk_server src/main.cpp)
target_link_libraries(bulk_server PRIVATE server)
if (MSVC)
  target_compile_definitions(server PRIVATE _WIN32_WINNT=0x0A00)
  target_compile_definitions(bulk_server PRIVATE _WIN32_WINNT=0x0A00)
else ()
  target_link_libraries(bulk_server PRIVATE -static)
endif()

install(TARGETS bulk_server
        RUNTIME DESTINATION bin
        COMPONENT "${PROJECT_NAME}")
if (PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_10_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
