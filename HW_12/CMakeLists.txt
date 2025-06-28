cmake_minimum_required(VERSION 4.0.3)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(mapreduce VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

if (MSVC OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if (PROJECT_IS_TOP_LEVEL)
    if (MSVC)
      set(CMAKE_CXX_STANDARD 23)
    else ()
      set(CMAKE_CXX_STANDARD 26)
      add_compile_options(-fcolor-diagnostics)
    endif()
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(CMAKE_CXX_EXTENSIONS OFF)
  endif()

  if (MSVC)
    add_compile_options(/W4)
  else()
    add_compile_options(-Wall -Wextra -pedantic)
  endif()

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

  add_executable(mapper mapper.cpp)
  add_executable(reducer reducer.cpp)
  add_executable(mapper_variance mapper_variance.cpp)
  add_executable(reducer_variance reducer_variance.cpp)
else()
  add_custom_target(mapreduce_all ALL
    COMMAND make -f ${CMAKE_CURRENT_SOURCE_DIR}/Makefile all
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building MapReduce using existing Makefile"
    VERBATIM)
endif()

enable_testing()

if (WIN32)
  add_test(
    NAME HW_12.test_mean_price
    COMMAND powershell -Command
    "cat ${CMAKE_CURRENT_SOURCE_DIR}/input/AB_NYC_2019.csv | \
    $<TARGET_FILE:mapper> | \
    sort | \
    $<TARGET_FILE:reducer> > ${CMAKE_CURRENT_BINARY_DIR}/output_mean ; \
    if (-not (Select-String -Pattern 'Mean price: 152.696' \
      -Path ${CMAKE_CURRENT_BINARY_DIR}/output_mean)) \
    { \
      Write-Host 'ERROR: Pattern not found: Mean price: 152.696' ; \
      Write-Host '--- Output file content:' ; \
      cat ${CMAKE_CURRENT_BINARY_DIR}/output_mean ; \
      exit 1 \
    }"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  add_test(
    NAME HW_12.test_variance
    COMMAND powershell -Command
    "cat ${CMAKE_CURRENT_SOURCE_DIR}/input/AB_NYC_2019.csv | \
    $<TARGET_FILE:mapper_variance> | \
    sort | \
    $<TARGET_FILE:reducer_variance> > ${CMAKE_CURRENT_BINARY_DIR}/output_variance ; \
    if (-not (Select-String -Pattern 'Variance: 57673.5' \
      -Path ${CMAKE_CURRENT_BINARY_DIR}/output_variance)) \
    { \
      Write-Host 'ERROR: Pattern not found: Variance: 57673.5' ; \
      Write-Host '--- Output file content:' ; \
      cat ${CMAKE_CURRENT_BINARY_DIR}/output_variance ; \
      exit 1 \
    }"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
else()
  add_test(
    NAME HW_12.test_mean_price
    COMMAND bash -c
    "cat ${CMAKE_CURRENT_SOURCE_DIR}/input/AB_NYC_2019.csv | \
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/mapper | \
    sort -k1 | \
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/reducer > ${CMAKE_CURRENT_BINARY_DIR}/output_mean && \
    grep -q 'Mean price: 152.696' ${CMAKE_CURRENT_BINARY_DIR}/output_mean"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  add_test(
    NAME HW_12.test_variance
    COMMAND bash -c
    "cat ${CMAKE_CURRENT_SOURCE_DIR}/input/AB_NYC_2019.csv | \
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/mapper_variance | \
    sort -k1 | \
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/reducer_variance > \
      ${CMAKE_CURRENT_BINARY_DIR}/output_variance && \
    grep -q 'Variance: 57673.5' ${CMAKE_CURRENT_BINARY_DIR}/output_variance"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/
        DESTINATION bin
        USE_SOURCE_PERMISSIONS
        COMPONENT "${PROJECT_NAME}")
if (PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  set(HW_12_DEB_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
