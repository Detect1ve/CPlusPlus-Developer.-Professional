cmake_minimum_required(VERSION 4.0.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(join_server VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  if (MSVC)
    set(SUPPORTED_CXX_STANDARDS 23 20)
  else()
    set(SUPPORTED_CXX_STANDARDS 26 23 20)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      add_compile_options(-fcolor-diagnostics)
    else()
      add_compile_options(-fdiagnostics-color=always)
    endif()
  endif()

  include(../cxx_standard.cmake)
  set(CMAKE_CXX_STANDARD ${CXX_STANDARD})
  set(CMAKE_CXX_STANDARD_REQUIRED True)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(../boost.cmake)
  include(../clang-tidy.cmake)
endif()

include_directories(include)

set(SOURCES
    src/main.cpp
    src/server.cpp
    src/database.cpp
)
add_executable(join_server ${SOURCES})
target_link_libraries(join_server PRIVATE Boost::asio)
if (MSVC)
  target_compile_definitions(join_server PRIVATE _WIN32_WINNT=0x0A00)
else ()
  target_link_libraries(join_server PRIVATE -static)
endif()

if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
  set_target_properties(join_server PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
      -altera-struct-pack-align,\
      -altera-unroll-loops,\
      -fuchsia-default-arguments-calls,\
      -llvmlibc-callee-namespace,\
      -llvmlibc-implementation-in-namespace,\
      -llvmlibc-restrict-system-libc-headers,\
      -misc-include-cleaner,\
      -misc-no-recursion,\
      -modernize-use-trailing-return-type;--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/include/.*")
endif()

install(TARGETS join_server
        RUNTIME DESTINATION bin
        COMPONENT "${PROJECT_NAME}")
if (PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_11_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
