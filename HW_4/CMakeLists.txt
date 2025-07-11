cmake_minimum_required(VERSION 4.0.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(print_ip VERSION ${PROJECT_VERSION} LANGUAGES CXX)

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

add_executable(print_ip src/main.cpp)
if (NOT MSVC)
  target_link_libraries(print_ip PRIVATE -static)
endif()

install(TARGETS print_ip RUNTIME DESTINATION bin
                                 COMPONENT "${PROJECT_NAME}")

find_package(Doxygen 1.14.0)
if (NOT Doxygen_FOUND)
  if (MSVC)
    set(DOXYGEN_URL https://github.com/doxygen/doxygen/releases/download/Release_1_14_0/doxygen-1.14.0.windows.x64.bin.zip)
    set(DOXYGEN_MD5 d7bd7814898ea345c2ee004c57ae0177)
  else()
    set(DOXYGEN_URL https://github.com/doxygen/doxygen/releases/download/Release_1_14_0/doxygen-1.14.0.linux.bin.tar.gz)
    set(DOXYGEN_MD5 e761a5097ae20ecccfd02041925f102a)
  endif()
  include(FetchContent)
  FetchContent_Declare(
    Doxygen
    URL ${DOXYGEN_URL}
    URL_MD5 ${DOXYGEN_MD5}
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    EXCLUDE_FROM_ALL)
  FetchContent_MakeAvailable(Doxygen)

  if (MSVC)
    set(DOXYGEN_EXECUTABLE "${doxygen_SOURCE_DIR}/doxygen.exe")
  else()
    set(DOXYGEN_EXECUTABLE "${doxygen_SOURCE_DIR}/bin/doxygen")
  endif()
  include(FindDoxygen)
endif()

set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs CACHE PATH "Path to Doxygen output directory")
set(DOXYGEN_NUM_PROC_THREADS 0)
set(DOXYGEN_WARN_AS_ERROR YES)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)

doxygen_add_docs(
  docs
  ${CMAKE_CURRENT_SOURCE_DIR}/src README.md
  COMMENT "Generating API documentation with Doxygen"
)

if (PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_4_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
